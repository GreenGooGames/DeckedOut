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

DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FItemSpawnRequestCompletedEvent, FGuid /*RequestId*/, TWeakObjectPtr<ATartarusItemBase> /*SpawnedItem*/)

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
	int32 GetItemId() { return ItemId; }
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
	// Datatable reference that contains all items.
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> ItemDataTable;

	/* 
	* Despawns an item if managed by this system.
	* Return: True if the item got destroyed, false if despawn failed.
	*/
	bool DespawnItem(ATartarusItemBase* const ToDespawn);

private:
	TArray<TObjectPtr<ATartarusItemBase>> ItemInstances;

#pragma region ASyncLoading
public:
	// Creates a request to spawn an item in the world.
	FGuid ASyncRequestSpawnItem(const int32 ItemId, const FTransform& SpawnTransform, FItemSpawnRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FItemSpawnRequestInfo* const SuccessRequest, TWeakObjectPtr<ATartarusItemBase> SpawnedItem);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FItemSpawnRequestInfo* const FailedRequest);

	// Called when the item datatable is loaded.
	void HandleDataTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

	/*
	* Creates a request to load an item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestLoadItem(const FItemTableRow* const ItemDefinition);

	// Called when the item blueprint is loaded.
	void HandleBlueprintLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

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
};
