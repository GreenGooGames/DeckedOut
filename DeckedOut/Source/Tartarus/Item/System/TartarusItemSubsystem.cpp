// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/System/TartarusItemSubsystem.h"

#include "Item/System/TartarusItemSubsystemSettings.h"
#include "Item/TartarusItemBase.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"
#include "System/TartarusHelpers.h"

#pragma region FSpawnItemsRequestInfo
FSpawnItemsRequestInfo::FSpawnItemsRequestInfo(const FTransform& Transform, const FItemSpawnRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	SpawnTransform = Transform;
	RequestCompletedEvent = OnCompleted;
}

void FSpawnItemsRequestInfo::AddAssetToLoad(const FSoftObjectPath& AssetPath, const int32 ItemId)
{
	AssetsToLoad.Add(AssetPath, ItemId);
}

TArray<FSoftObjectPath> FSpawnItemsRequestInfo::GetAssetsToLoad()
{
	TArray<FSoftObjectPath> Assets;

	for (auto Asset : AssetsToLoad)
	{
		Assets.Add(Asset.Key);
	}

	return Assets;
}

int32 FSpawnItemsRequestInfo::GetItemId(const UObject* const Asset)
{
	for (auto AssetToIdPair : AssetsToLoad)
	{
		if (AssetToIdPair.Key.ResolveObject() == Asset)
		{
			return AssetToIdPair.Value;
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
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to despawn item: The item was invalid!"), __FUNCTION__);
		return false;
	}

	const int32 RemovedIndex = ItemInstances.RemoveSingleSwap(ToDespawn);

	if (RemovedIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to despawn item: The item was not found in the instance list!"), __FUNCTION__);
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn an item!"), __FUNCTION__);
		return false;
	}

	ItemInstance->SetReferenceId(ItemId);
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
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
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
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to include handle for async loading: RowHandle does not reference an item!"), __FUNCTION__);
			continue;
		}

		SpawnRequest.AddAssetToLoad(ItemRow->Blueprint.ToSoftObjectPath(), ItemRow->UniqueItemId);
	}

	// Create a callback for when the items are loaded.
	FAsyncLoadAssetRequestCompletedEvent OnItemsLoaded;
	OnItemsLoaded.AddUObject(this, &UTartarusItemSubsystem::HandleItemsLoaded);

	// Create a request to start loading the Items.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAssets(SpawnRequest.GetAssetsToLoad(), OnItemsLoaded);
	SpawnRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), __FUNCTION__);
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
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Create a request to handle spawning the items;
	FSpawnItemsRequestInfo SpawnRequest = FSpawnItemsRequestInfo(SpawnTransform, OnRequestCompleted);

	// Gather all the paths of the assets to load.
	for (const FItemTableRow& ItemRow : ItemTableRows)
	{
		SpawnRequest.AddAssetToLoad(ItemRow.Blueprint.ToSoftObjectPath(), ItemRow.UniqueItemId);
	}

	// Create a callback for when the items are loaded.
	FAsyncLoadAssetRequestCompletedEvent OnItemsLoaded;
	OnItemsLoaded.AddUObject(this, &UTartarusItemSubsystem::HandleItemsLoaded);

	// Create a request to start loading the Items.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAssets(SpawnRequest.GetAssetsToLoad(), OnItemsLoaded);
	SpawnRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), __FUNCTION__);
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	// Get all loaded Assets, loaded children will be ignored later.
	TArray<UObject*> LoadedAssets;
	StreamingHandle->GetLoadedAssets(LoadedAssets);

	// Convert each asset to an item subclass, and ignore any loaded children.
	TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItens;

	for (UObject* const Asset : LoadedAssets)
	{
		TSubclassOf<ATartarusItemBase> AsItemClass = Cast<UClass>(Asset);

		if (!IsValid(AsItemClass))
		{
			continue;
		}

		// The asset is a valid item class, spawn it in the world.
		TWeakObjectPtr<ATartarusItemBase> SpawnedItem = SpawnItem(AsItemClass, CurrentRequest->GetItemId(Asset), CurrentRequest->GetSpawnTransform());

		if (!IsValid(SpawnedItem.Get()))
		{
			continue;
		}

		SpawnedItens.Add(SpawnedItem);
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
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
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
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: No async load started!"), __FUNCTION__);
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), __FUNCTION__);
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
