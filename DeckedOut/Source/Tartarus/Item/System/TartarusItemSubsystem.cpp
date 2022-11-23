// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/System/TartarusItemSubsystem.h"

#include "Item/System/TartarusItemSubsystemSettings.h"
#include "Item/TartarusItemBase.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"

#pragma region FSpawnItemsRequestInfo
FSpawnItemsRequestInfo::FSpawnItemsRequestInfo(const FTransform& Transform, const FItemSpawnRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	SpawnTransform = Transform;
	RequestCompletedEvent = OnCompleted;
}

void FSpawnItemsRequestInfo::AddItemToLoad(const FSoftObjectPath& ItemObjectPath, const int32 ItemId)
{
	FLoadingItemData* const  ExistingLoad = ItemsToLoad.FindByPredicate([&ItemId](const FLoadingItemData& ItemData)
		{
			return ItemData.ItemId == ItemId;
		});

	if (ExistingLoad) // Increase the count of instances to spawn of this Item.
	{
		ExistingLoad->Count++;
	}
	else // Create a new entry and add it.
	{
		FLoadingItemData LoadData = FLoadingItemData();
		LoadData.ItemId = ItemId;
		LoadData.ObjectPath = ItemObjectPath;
		LoadData.Count = 1;

		ItemsToLoad.Add(LoadData);
	}
}

TArray<FSoftObjectPath> FSpawnItemsRequestInfo::GetAssetsToLoad() const
{
	TArray<FSoftObjectPath> Assets;

	for (const FLoadingItemData& ItemData : ItemsToLoad)
	{
		Assets.Add(ItemData.ObjectPath);
	}

	return Assets;
}

int32 FSpawnItemsRequestInfo::GetItemId(const UObject* const Asset) const
{
	for (const FLoadingItemData& ItemData : ItemsToLoad)
	{
		if (ItemData.ObjectPath.ResolveObject() == Asset)
		{
			return ItemData.ItemId;
		}
	}

	return FTartarusHelpers::InvalidItemId;
}
#pragma endregion

#pragma region FGetItemDataRequestInfo
FGetItemDataRequestInfo::FGetItemDataRequestInfo(const TArray<int32>& ItemIdsToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	ItemIds = ItemIdsToLoad;
	RequestCompletedEvent = OnCompleted;
}
#pragma endregion

UTartarusItemSubsystem::UTartarusItemSubsystem()
{
	const UTartarusItemSubsystemSettings* const Settings = GetDefault<UTartarusItemSubsystemSettings>();

	if (IsValid(Settings))
	{
		ItemDataTable = Settings->ItemDataTable;
	}
}

bool UTartarusItemSubsystem::DespawnItem(ATartarusItemBase* const ToDespawn)
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
	ATartarusItemBase* const AsItem = Cast<ATartarusItemBase>(ToDespawn);

	return DespawnItem(AsItem);
}

TWeakObjectPtr<ATartarusItemBase> UTartarusItemSubsystem::SpawnItem(const TSubclassOf<ATartarusItemBase>& ItemClass, const int32 ItemId, const FTransform& SpawnTransform)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	// Spawn the Item in the world.
	ATartarusItemBase* const ItemInstance = GetWorld()->SpawnActor<ATartarusItemBase>(ItemClass, SpawnTransform, SpawnParams);

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
FGuid UTartarusItemSubsystem::AsyncRequestSpawnItems(const TArray<FDataTableRowHandle>& ItemTableHandles, const FTransform& SpawnTransform, const FItemSpawnRequestCompletedEvent& OnRequestCompleted)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle spawning the items;
	FSpawnItemsRequestInfo SpawnRequest = FSpawnItemsRequestInfo(SpawnTransform, OnRequestCompleted);

	// Gather all the paths of the assets to load.
	for (const FDataTableRowHandle& RowHandle : ItemTableHandles)
	{
		FString ContextString = "";
		const FItemTableRow* const ItemRow = RowHandle.GetRow<FItemTableRow>(ContextString);

		if (!ItemRow)
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to include handle for async loading: RowHandle does not reference an item!"), *FString(__FUNCTION__));
			continue;
		}

		SpawnRequest.AddItemToLoad(ItemRow->Blueprint.ToSoftObjectPath(), ItemRow->UniqueItemId);
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

FGuid UTartarusItemSubsystem::AsyncRequestSpawnItems(const TArray<FItemTableRow>& ItemTableRows, const FTransform& SpawnTransform, const FItemSpawnRequestCompletedEvent& OnRequestCompleted)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle spawning the items;
	FSpawnItemsRequestInfo SpawnRequest = FSpawnItemsRequestInfo(SpawnTransform, OnRequestCompleted);

	// Gather all the paths of the assets to load.
	for (const FItemTableRow& ItemRow : ItemTableRows)
	{
		SpawnRequest.AddItemToLoad(ItemRow.Blueprint.ToSoftObjectPath(), ItemRow.UniqueItemId);
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

void UTartarusItemSubsystem::HandleSpawnItemRequestCompleted(const FSpawnItemsRequestInfo* const CompletedRequest, const TArray<TWeakObjectPtr<ATartarusItemBase>>& SpawnedItems)
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

	// For each item that is loaded, spawn the number of requested instances.
	TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItens;

	for (const FLoadingItemData& LoadingData : CurrentRequest->GetItemsToLoad())
	{
		TSubclassOf<ATartarusItemBase> AsItemClass = Cast<UClass>(LoadingData.ObjectPath.ResolveObject());

		if (!IsValid(AsItemClass))
		{
			continue;
		}

		for (int32 i = 0; i < LoadingData.Count; i++)
		{
			TWeakObjectPtr<ATartarusItemBase> SpawnedItem = SpawnItem(AsItemClass, LoadingData.ItemId, CurrentRequest->GetSpawnTransform());

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
FGuid UTartarusItemSubsystem::AsyncRequestGetItemsData(const TArray<int32>& ItemIds, const FGetItemDataRequestCompletedEvent& OnRequestCompleted)
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

void UTartarusItemSubsystem::HandleGetItemsDataRequestCompleted(const FGetItemDataRequestInfo* const CompletedRequest, TArray<FItemTableRow> ItemsData)
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	// Gather all the requested rows.
	TArray<FItemTableRow> ItemsData;
	FString ContextString = "";
	TArray<FItemTableRow*> AllItemRows;

	ItemDataTable.Get()->GetAllRows<FItemTableRow>(ContextString, AllItemRows);

	for (const int32& ItemId : CurrentRequest->GetItemIds())
	{
		FItemTableRow* ItemRow = nullptr;

		for (FItemTableRow* const Row : AllItemRows)
		{
			if (Row->UniqueItemId != ItemId)
			{
				continue;
			}

			ItemRow = Row;
		}

		if (!ItemRow)
		{
			continue;
		}

		ItemsData.Add(*ItemRow);
	}

	HandleGetItemsDataRequestCompleted(CurrentRequest, ItemsData);
}
#pragma endregion
