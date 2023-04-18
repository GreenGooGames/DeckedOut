// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Item/System/TartarusAsyncItemData.h"
#include "Subsystems/WorldSubsystem.h"

#include "TartarusItemSubsystem.generated.h"

class UTartarusItem;
class ATartarusItemInstance;
class UDataTable;

struct FStreamableHandle;

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
	bool DespawnItem(ATartarusItemInstance* const ToDespawn);

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
	TWeakObjectPtr<ATartarusItemInstance> SpawnItem(const TSubclassOf<ATartarusItemInstance>& ItemClass, const FPrimaryAssetId ItemId, const FTransform& SpawnTransform);

private:
	TArray<TObjectPtr<ATartarusItemInstance>> ItemInstances;

#pragma region ASyncSpawn
public:
	/*
	* Creates an ASync request to load and spawn Items in the world.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestSpawnItems(const TArray<UTartarusItem*>& ItemsData, const FTransform& SpawnTransform, const FItemSpawnParameters& SpawnParameters, const FItemSpawnRequestCompletedEvent& OnRequestCompleted);

protected:
	// Executed when a Item spawn request has completed.
	void HandleSpawnItemRequestCompleted(const FSpawnItemsRequestInfo* const CompletedRequest, const TArray<TWeakObjectPtr<ATartarusItemInstance>>& SpawnedItems);

	// Callback for when an Items load request completed.
	void HandleItemsLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle);

private:
	TArray<FSpawnItemsRequestInfo> SpawnItemRequests;
#pragma endregion

#pragma region ASyncItemData
public:
	/*
	* Creates a request to load the item Assets.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestGetItemsData(const TArray<FPrimaryAssetId>& ItemIds, const FGetItemDataRequestCompletedEvent& OnRequestCompleted);

	/*
	* Creates a request to load the items datatable and provide the TableRows of the requested item types.
	* Return: The unique identifier of the request.
	*/
	FGuid AsyncRequestGetItemsData(const TArray<EItemType>& ItemTypes, const FGetItemDataRequestCompletedEvent& OnRequestCompleted);

protected:
	void HandleGetItemsDataRequestCompleted(const FGetItemDataRequestInfo* const CompletedRequest, TArray<UTartarusItem*>& ItemsData);
	void HandleItemsDataTableLoaded(const FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle);

private:
	TArray<FGetItemDataRequestInfo> GetItemDataRequests;
#pragma endregion
};
