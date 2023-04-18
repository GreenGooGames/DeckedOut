// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/System/TartarusItemSubsystem.h"

#include "Engine/World.h"
#include "Item/System/TartarusItemSubsystemSettings.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Item/TartarusItemInstance.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"

UTartarusItemSubsystem::UTartarusItemSubsystem()
{
	const UTartarusItemSubsystemSettings* const Settings = GetDefault<UTartarusItemSubsystemSettings>();

	if (IsValid(Settings))
	{
		ItemDataTable = Settings->ItemDataTable;
	}
}

bool UTartarusItemSubsystem::DespawnItem(ATartarusItemInstance* const ToDespawn)
{
	if (!IsValid(ToDespawn))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to despawn item: The item was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const int32 RemovedIndex = ItemInstances.RemoveSingleSwap(ToDespawn);
	if (RemovedIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to despawn item: The item was not found in the instance list!"), *FString(__FUNCTION__));
		return false;
	}

	ToDespawn->Destroy();

	return true;
}

bool UTartarusItemSubsystem::DespawnItem(AActor* const ToDespawn)
{
	ATartarusItemInstance* const AsItem = Cast<ATartarusItemInstance>(ToDespawn);

	return DespawnItem(AsItem);
}

TWeakObjectPtr<ATartarusItemInstance> UTartarusItemSubsystem::SpawnItem(const TSubclassOf<ATartarusItemInstance>& ItemClass, const FPrimaryAssetId ItemId, const FTransform& SpawnTransform)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	// Spawn the Item in the world.
	ATartarusItemInstance* const ItemInstance = GetWorld()->SpawnActor<ATartarusItemInstance>(ItemClass, SpawnTransform, SpawnParams);
	if (!ItemInstance)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn an item!"), *FString(__FUNCTION__));
		return false;
	}

	ItemInstance->SetItemId(ItemId);
	ItemInstances.Add(ItemInstance);

	return ItemInstance;
}

#pragma region ASyncSpawn
FGuid UTartarusItemSubsystem::AsyncRequestSpawnItems(const TArray<UTartarusItem*>& ItemsData, const FTransform& SpawnTransform, const FItemSpawnParameters& SpawnParameters, const FItemSpawnRequestCompletedEvent& OnRequestCompleted)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle spawning the items;
	FSpawnItemsRequestInfo SpawnRequest = FSpawnItemsRequestInfo(SpawnTransform, SpawnParameters, OnRequestCompleted);

	// Gather all the Item instances to load.
	for (const UTartarusItem* ItemData : ItemsData)
	{
		if (!IsValid(ItemData))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to include asset for async loading: ItemData is invalid or does not reference a Blueprint!"), *FString(__FUNCTION__));
			continue;
		}

		SpawnRequest.AddItemToLoad(ItemData->Blueprint.ToSoftObjectPath(), ItemData->GetPrimaryAssetId());
	}

	// Create a callback for when the items are loaded.
	FAsyncLoadAssetRequestCompletedEvent OnItemsLoaded;
	OnItemsLoaded.AddUObject(this, &UTartarusItemSubsystem::HandleItemsLoaded);

	// Create a request to start loading the Items.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAssets(SpawnRequest.GetAssetsToLoad(), OnItemsLoaded);
	SpawnRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), *FString(__FUNCTION__));
		return FGuid();
	}

	SpawnItemRequests.Add(SpawnRequest);

	return SpawnRequest.GetRequestId();
}

void UTartarusItemSubsystem::HandleSpawnItemRequestCompleted(const FSpawnItemsRequestInfo* const CompletedRequest, const TArray<TWeakObjectPtr<ATartarusItemInstance>>& SpawnedItems)
{
	if (!CompletedRequest)
	{
		return;
	}

	CompletedRequest->OnItemSpawnRequestCompleted().Broadcast(CompletedRequest->GetRequestId(), SpawnedItems);
	SpawnItemRequests.RemoveSingleSwap(*CompletedRequest);
}

void UTartarusItemSubsystem::HandleItemsLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle)
{
	// Get the request that is being handled.
	FSpawnItemsRequestInfo* const CurrentRequest = SpawnItemRequests.FindByPredicate([&ASyncLoadRequestId](const FSpawnItemsRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedItens;
	const int32 NumItemsToSpawn = CurrentRequest->GetItemsToLoad().Num();

	for (int32 i = 0; i < NumItemsToSpawn; i++)
	{
		FTransform SpawnTransform = CurrentRequest->GetSpawnTransform();

		// Calculate the position to spawn at if required.
		switch (CurrentRequest->GetSpawnParameters().GetSpawnMethod())
		{
		case EItemSpawnMethod::Orbit:
		{
			const float Interval = (2 * PI) / NumItemsToSpawn;
			const float OrbitAngle = Interval * i;
			const FVector2D UnitOffset = FVector2D(cos(OrbitAngle), sin(OrbitAngle));
			const FVector2D OrbitOffset = UnitOffset * CurrentRequest->GetSpawnParameters().GetOrbitRadius();

			SpawnTransform.SetLocation(SpawnTransform.GetLocation() + FVector(OrbitOffset, 0));
			break;
		}
		default:
			break;
		}

		// Retrieve the class to spawn and spawn it in the world.
		const FLoadingItemData& LoadingData = CurrentRequest->GetItemsToLoad()[i];
		TSubclassOf<ATartarusItemInstance> AsItemClass = Cast<UClass>(LoadingData.ObjectPath.ResolveObject());
		if (!IsValid(AsItemClass))
		{
			continue;
		}

		// For each item that is loaded, spawn the number of requested instances.
		for (int32 j = 0; j < LoadingData.Count; j++)
		{
			TWeakObjectPtr<ATartarusItemInstance> SpawnedItem = SpawnItem(AsItemClass, LoadingData.ItemId, SpawnTransform);
			if (!IsValid(SpawnedItem.Get()))
			{
				continue;
			}

			SpawnedItens.Add(SpawnedItem);
		}
	}

	HandleSpawnItemRequestCompleted(CurrentRequest, SpawnedItens);
}
#pragma endregion

#pragma region ASyncItemData
FGuid UTartarusItemSubsystem::AsyncRequestGetItemsData(const TArray<FPrimaryAssetId>& ItemIds, const FGetItemDataRequestCompletedEvent& OnRequestCompleted)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	// Create a request to handle spawning the items;
	FGetItemDataRequestInfo GetItemDataRequest = FGetItemDataRequestInfo(ItemIds, OnRequestCompleted);
	
	// Create a callback for when the DataTable is loaded.
	FAsyncLoadAssetRequestCompletedEvent OnDataTableLoaded;
	OnDataTableLoaded.AddUObject(this, &UTartarusItemSubsystem::HandleItemsDataTableLoaded);
	
	// Create a request to start loading the Item Table.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDataTable.ToSoftObjectPath(), OnDataTableLoaded);
	GetItemDataRequest.SetASyncLoadRequestId(AsyncLoadRequestId);
	
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	GetItemDataRequests.Add(GetItemDataRequest);
	
	return GetItemDataRequest.GetRequestId();
}

FGuid UTartarusItemSubsystem::AsyncRequestGetItemsData(const TArray<EItemType>& ItemTypes, const FGetItemDataRequestCompletedEvent& OnRequestCompleted)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	// Create a request to handle spawning the items;
	FGetItemDataRequestInfo GetItemDataRequest = FGetItemDataRequestInfo(ItemTypes, OnRequestCompleted);
	
	// Create a callback for when the DataTable is loaded.
	FAsyncLoadAssetRequestCompletedEvent OnDataTableLoaded;
	OnDataTableLoaded.AddUObject(this, &UTartarusItemSubsystem::HandleItemsDataTableLoaded);
	
	// Create a request to start loading the Item Table.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDataTable.ToSoftObjectPath(), OnDataTableLoaded);
	GetItemDataRequest.SetASyncLoadRequestId(AsyncLoadRequestId);
	
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	GetItemDataRequests.Add(GetItemDataRequest);
	
	return GetItemDataRequest.GetRequestId();
}

void UTartarusItemSubsystem::HandleGetItemsDataRequestCompleted(const FGetItemDataRequestInfo* const CompletedRequest, TArray<UTartarusItem*>& ItemsData)
{
	if (!CompletedRequest)
	{
		return;
	}

	CompletedRequest->OnGetItemDataRequestCompleted().Broadcast(CompletedRequest->GetRequestId(), ItemsData);
	GetItemDataRequests.RemoveSingleSwap(*CompletedRequest);
}

void UTartarusItemSubsystem::HandleItemsDataTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> StreamingHandle)
{
	// Get the request that is being handled.
	FGetItemDataRequestInfo* const CurrentRequest = GetItemDataRequests.FindByPredicate([&ASyncLoadRequestId](const FGetItemDataRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});
	
	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Get Item data failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}
	
	// Gather all the requested rows.
	TArray<UTartarusItem*> ItemsData;
	FString ContextString = "";
	TArray<FItemTableRow*> AllItemRows;
	UDataTable* const LoadedTable = ItemDataTable.Get();

	LoadedTable->GetAllRows<FItemTableRow>(ContextString, AllItemRows);
	
	for (const FItemTableRow* const ItemRow : AllItemRows)
	{
		// Does this Item Data row identify as any of the elements to search for?
		const bool bContainsId = CurrentRequest->GetItemIds().Contains(ItemRow->ItemData->GetPrimaryAssetId());
		const bool bContainsType = CurrentRequest->GetItemTypes().Contains(ItemRow->ItemData->ItemType);
		
		// If any one condition is satisfied, add it to ItemsData.
		if (!bContainsId && !bContainsType)
		{
			continue;
		}
	
		ItemsData.Add(ItemRow->ItemData);
	}
	
	HandleGetItemsDataRequestCompleted(CurrentRequest, ItemsData);
}
#pragma endregion
