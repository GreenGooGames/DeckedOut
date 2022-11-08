// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "System/TartarusASyncLoadData.h"
#include "System/TartarusHelpers.h"

#include "TartarusItemSubsystem.generated.h"

class ATartarusItemBase;
class UDataTable;

struct FStreamableHandle;
struct FItemTableRow;

DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FLoadItemTableRequestCompletedEvent, FGuid /*RequestId*/, TWeakObjectPtr<UDataTable> /*ItemTable*/);
DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FItemSpawnRequestCompletedEvent, FGuid /*RequestId*/, TWeakObjectPtr<ATartarusItemBase> /*SpawnedItem*/)

USTRUCT()
struct FLoadItemTableRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FLoadItemTableRequestInfo() {}
	FLoadItemTableRequestInfo(const FLoadItemTableRequestCompletedEvent& OnCompleted)
	{
		RequestId = FGuid::NewGuid();

		OnRequestCompleteEvent = OnCompleted;
	}

	const FLoadItemTableRequestCompletedEvent& OnLoadItemTableRequestCompleted() const { return OnRequestCompleteEvent; }

private:
	FLoadItemTableRequestCompletedEvent OnRequestCompleteEvent = FLoadItemTableRequestCompletedEvent();

};

USTRUCT()
struct FItemSpawnRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FItemSpawnRequestInfo() {}
	FItemSpawnRequestInfo(const FItemSpawnRequestCompletedEvent& OnCompleted, const int32 ItemToSpawnId, const FTransform& ItemSpawnTransform)
	{
		RequestId = FGuid::NewGuid();

		OnRequestCompleteEvent = OnCompleted;
		ItemId = ItemToSpawnId;
		SpawnTransform = ItemSpawnTransform;
	}
	
	const FItemSpawnRequestCompletedEvent& OnItemSpawnRequestCompleted() const { return OnRequestCompleteEvent; }
	int32 GetItemId() const { return ItemId; }
	const FTransform& GetSpawnTransform() const { return SpawnTransform; }

private:
	FItemSpawnRequestCompletedEvent OnRequestCompleteEvent = FItemSpawnRequestCompletedEvent();

	int32 ItemId = FTartarusHelpers::InvalidItemId;
	FTransform SpawnTransform = FTransform();
};

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UTartarusItemSubsystem();

public:
	/* 
	* Despawns an item if managed by this system.
	* Return: True if the item got destroyed, false if despawn failed.
	*/
	bool DespawnItem(ATartarusItemBase* const ToDespawn);

protected:
	// Datatable reference that contains all items.
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> ItemDataTable;

private:
	TArray<TObjectPtr<ATartarusItemBase>> ItemInstances;

#pragma region ASyncLoading

#pragma region SpawnItem
public:
	// Creates a request to spawn an item in the world.
	FGuid ASyncRequestSpawnItem(const int32 ItemId, const FTransform& SpawnTransform, FItemSpawnRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleSpawnItemRequestSuccess(const FItemSpawnRequestInfo* const SuccessRequest, TWeakObjectPtr<ATartarusItemBase> SpawnedItem);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleSpawnItemRequestFailed(const FItemSpawnRequestInfo* const FailedRequest);

	// Called when the item datatable is loaded for a spawn request.
	void HandleItemTableLoadCompleted(FGuid ASyncLoadRequestId, TWeakObjectPtr<UDataTable> ItemTable);

	/*
	* Creates a request to load an item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestLoadItem(const FItemTableRow* const ItemDefinition);

	// Called when the item blueprint is loaded.
	void HandleItemLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

	/* Creates and actor of the class using the provided transform in the world.
	* Return: True if an item was spawned, false if not item was spawned.
	*/
	ATartarusItemBase* SpawnItem(TSubclassOf<ATartarusItemBase>& ItemClass, const FTransform& SpawnTransform);

	/* Searches for the Item row based on the given Id.
	* Return: Row of the item from the database, nullptr if not found.
	*/
	FItemTableRow* FindItemRow(UDataTable* const DataTable, const int32 ItemId) const;

private:
	TArray<FItemSpawnRequestInfo> SpawnRequests;
#pragma endregion

#pragma region LoadItemTable
public:
	FGuid ASyncRequestLoadItemTable(FLoadItemTableRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	void HandleItemTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

	void HandleItemTableLoadRequestSuccess(const FLoadItemTableRequestInfo* const SuccessRequest);
	void HandleItemTableLoadRequestFailed(const FLoadItemTableRequestInfo* const FailedRequest);

private:
	TArray<FLoadItemTableRequestInfo> LoadItemTableRequests;
#pragma endregion

#pragma endregion
};
