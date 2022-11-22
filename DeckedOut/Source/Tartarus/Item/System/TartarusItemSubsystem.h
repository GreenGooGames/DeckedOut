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

#pragma region ASyncSpawn
DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FItemSpawnRequestCompletedEvent, FGuid /*RequestId*/, TArray<TWeakObjectPtr<ATartarusItemBase>> /*SpawnedItems*/)

USTRUCT()
struct FLoadingItemData
{
	GENERATED_BODY()

public:
	int32 ItemId = FTartarusHelpers::InvalidItemId;
	int32 Count = 0;
	FSoftObjectPath ObjectPath = FSoftObjectPath();
};

USTRUCT()
struct FSpawnItemsRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FSpawnItemsRequestInfo() {}
	FSpawnItemsRequestInfo(const FTransform& Transform, const FItemSpawnRequestCompletedEvent& OnCompleted);

	void AddItemToLoad(const FSoftObjectPath& ItemObjectPath, const int32 ItemId);
	TArray<FSoftObjectPath> GetAssetsToLoad() const;
	const TArray<FLoadingItemData>& GetItemsToLoad() const { return ItemsToLoad; };

	int32 GetItemId(const UObject* const Asset) const;

	const FTransform& GetSpawnTransform() const { return SpawnTransform; }
	const FItemSpawnRequestCompletedEvent& OnItemSpawnRequestCompleted() const { return RequestCompletedEvent; }

private:
	FTransform SpawnTransform = FTransform();
	FItemSpawnRequestCompletedEvent RequestCompletedEvent = FItemSpawnRequestCompletedEvent();
	TArray<FLoadingItemData> ItemsToLoad;
};
#pragma endregion

#pragma region ASyncItemData
DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FGetItemDataRequestCompletedEvent, FGuid /*RequestId*/, TArray<FItemTableRow> /*ItemsData*/)

USTRUCT()
struct FGetItemDataRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FGetItemDataRequestInfo() {}
	FGetItemDataRequestInfo(const TArray<int32>& ItemIdsToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted);

	const FGetItemDataRequestCompletedEvent& OnGetItemDataRequestCompleted() const { return RequestCompletedEvent; }
	const TArray<int32>& GetItemIds() { return ItemIds; }

private:
	FGetItemDataRequestCompletedEvent RequestCompletedEvent = FGetItemDataRequestCompletedEvent();
	TArray<int32> ItemIds;
};
#pragma endregion

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

	/*
	* Despawns an item if managed by this system.
	* Return: True if the item got destroyed, false if despawn failed.
	*/
	bool DespawnItem(AActor* const ToDespawn);

protected:
	// Datatable reference that contains all items.
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> ItemDataTable;

	/*
	* Spawns an item in the world.
	* Return: The item that is spawned.
	*/
	TWeakObjectPtr<ATartarusItemBase> SpawnItem(const TSubclassOf<ATartarusItemBase>& ItemClass, const int32 ItemId, const FTransform& SpawnTransform);

private:
	TArray<TObjectPtr<ATartarusItemBase>> ItemInstances;

#pragma region ASyncSpawn
public:
	/*
	* Creates an ASync request to load and spawn Items in the world.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestSpawnItems(const TArray<FDataTableRowHandle>& ItemTableHandles, const FTransform& SpawnTransform, const FItemSpawnRequestCompletedEvent& OnRequestCompleted);

	/*
	* Creates an ASync request to load and spawn Items in the world.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestSpawnItems(const TArray<FItemTableRow>& ItemTableRows, const FTransform& SpawnTransform, const FItemSpawnRequestCompletedEvent& OnRequestCompleted);

protected:
	// Executed when a Item spawn request has completed.
	void HandleSpawnItemRequestCompleted(const FSpawnItemsRequestInfo* const CompletedRequest, const TArray<TWeakObjectPtr<ATartarusItemBase>>& SpawnedItems);

	// Callback for when an Items load request completed.
	void HandleItemsLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle);

private:
	TArray<FSpawnItemsRequestInfo> SpawnItemRequests;
#pragma endregion

#pragma region ASyncItemData
public:
	/*
	* Creates a request to load the items datatable and provide the TableRows of the requested items.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestGetItemsData(const TArray<int32>& ItemIds, const FGetItemDataRequestCompletedEvent& OnRequestCompleted);

protected:
	void HandleGetItemsDataRequestCompleted(const FGetItemDataRequestInfo* const CompletedRequest, TArray<FItemTableRow> ItemsData);
	void HandleItemsDataTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle);

private:
	TArray<FGetItemDataRequestInfo> GetItemDataRequests;
#pragma endregion
};
