// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/System/TartarusItemSubsystem.h"

#include "Item/System/TartarusItemSubsystemSettings.h"
#include "Item/TartarusItemBase.h"
#include "Item/TartarusItemData.h"
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

#pragma region ASyncLoading
#pragma region SpawnItem
FGuid UTartarusItemSubsystem::ASyncRequestSpawnItem(const int32 ItemId, const FTransform& SpawnTransform, FItemSpawnRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}
	
	// Create a callback for when the item table is loaded.
	FLoadItemTableRequestCompletedEvent OnItemTableLoaded;
	OnItemTableLoaded.AddUObject(this, &ThisClass::HandleItemTableLoadCompleted);

	// Create a request to start loading the Item Table.
	const FGuid AsyncLoadRequestId = ASyncRequestLoadItemTable(OnItemTableLoaded);

	// Setup the request for this spawn.
	FItemSpawnRequestInfo SpawnRequest = FItemSpawnRequestInfo(OnRequestCompletedEvent, ItemId, SpawnTransform);
	SpawnRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	SpawnRequests.Add(SpawnRequest);

	return SpawnRequest.GetRequestId();
}

void UTartarusItemSubsystem::HandleSpawnItemRequestSuccess(const FItemSpawnRequestInfo* const SuccessRequest, TWeakObjectPtr<ATartarusItemBase> SpawnedItem)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnItemSpawnRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), SpawnedItem);
	SpawnRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusItemSubsystem::HandleSpawnItemRequestFailed(const FItemSpawnRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnItemSpawnRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	SpawnRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusItemSubsystem::HandleItemTableLoadCompleted(FGuid ASyncLoadRequestId, TWeakObjectPtr<UDataTable> ItemTable)
{
	// Get the request that is being handled.
	FItemSpawnRequestInfo* const CurrentRequest = SpawnRequests.FindByPredicate([&ASyncLoadRequestId](const FItemSpawnRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});
	
	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), __FUNCTION__);
		return;
	}
	
	// Verify the table is loaded.	
	if (!IsValid(ItemTable.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: datatable was not loaded!"), __FUNCTION__);
		HandleSpawnItemRequestFailed(CurrentRequest);

		return;
	}
	
	// Get the Item that was requestes from the DataTable
	FItemTableRow* const ItemDefinition = FindItemRow(ItemTable.Get(), CurrentRequest->GetItemId());
	
	if (!ItemDefinition)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Item was not found in the DataTable!"), __FUNCTION__);
		HandleSpawnItemRequestFailed(CurrentRequest);

		return;
	}
	
	// Load the Item asynchronously.
	FGuid ItemLoadRequestId = AsyncRequestLoadItem(ItemDefinition);

	if (!ItemLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not load the item async!"), __FUNCTION__);
		HandleSpawnItemRequestFailed(CurrentRequest);
		
		return;
	}
	
	CurrentRequest->SetASyncLoadRequestId(ItemLoadRequestId);
}

FGuid UTartarusItemSubsystem::AsyncRequestLoadItem(const FItemTableRow* const ItemDefinition)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusItemSubsystem::HandleItemLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDefinition->Blueprint.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	return AsyncLoadRequestId;
}

void UTartarusItemSubsystem::HandleItemLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FItemSpawnRequestInfo* const CurrentRequest = SpawnRequests.FindByPredicate([&ASyncLoadRequestId](const FItemSpawnRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), __FUNCTION__);

		return;
	}

	TSubclassOf<ATartarusItemBase> ItemClass = Cast<UClass>(AssetHandle.Get()->GetLoadedAsset());
	
	if (!IsValid(ItemClass))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Class failed to load!"), __FUNCTION__);
		HandleSpawnItemRequestFailed(CurrentRequest);
		
		return;
	}

	ATartarusItemBase* const SpawnedItem = SpawnItem(ItemClass, CurrentRequest->GetSpawnTransform());

	if (!IsValid(SpawnedItem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Item failed to spawn!"), __FUNCTION__);
		HandleSpawnItemRequestFailed(CurrentRequest);

		return;
	}

	SpawnedItem->SetReferenceId(CurrentRequest->GetItemId());

	HandleSpawnItemRequestSuccess(CurrentRequest, SpawnedItem);
}

ATartarusItemBase* UTartarusItemSubsystem::SpawnItem(TSubclassOf<ATartarusItemBase>& ItemClass, const FTransform& SpawnTransform)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	// Spawn the treasure in the world.
	ATartarusItemBase* const ItemInstance = GetWorld()->SpawnActor<ATartarusItemBase>(ItemClass, SpawnTransform, SpawnParams);

	if (!ItemInstance)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn an item!"), __FUNCTION__);
		return nullptr;
	}

	ItemInstances.Add(ItemInstance);

	return ItemInstance;
}

FItemTableRow* UTartarusItemSubsystem::FindItemRow(UDataTable* const DataTable, const int32 ItemId) const
{
	if (!IsValid(DataTable))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: database is nullptr!"), __FUNCTION__);
		return nullptr;
	}

	// Retrieve all rows in the datatable.
	FString ContextString = "";
	TArray<FItemTableRow*> DataTableRows;

	DataTable->GetAllRows<FItemTableRow>(ContextString, DataTableRows);

	// Iterate all rows untill the wanted one is found.
	for (FItemTableRow* const ItemTableRow : DataTableRows)
	{
		if (ItemTableRow->UniqueId == ItemId)
		{
			return ItemTableRow;
		}
	}

	return nullptr;
}
#pragma endregion

#pragma region LoadItemTable
FGuid UTartarusItemSubsystem::ASyncRequestLoadItemTable(FLoadItemTableRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusItemSubsystem::HandleItemTableLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDataTable.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	// Setup the request for this spawn.
	FLoadItemTableRequestInfo LoadRequest = FLoadItemTableRequestInfo(OnRequestCompletedEvent);
	LoadRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	LoadItemTableRequests.Add(LoadRequest);

	return LoadRequest.GetRequestId();
}

void UTartarusItemSubsystem::HandleItemTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FLoadItemTableRequestInfo* const CurrentRequest = LoadItemTableRequests.FindByPredicate([&ASyncLoadRequestId](const FLoadItemTableRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	// Store the loaded asset for future references.
	ItemDataTable = AssetHandle.Get()->GetLoadedAsset();

	if (!IsValid(ItemDataTable.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Load ItemTable failed: Item table was invalid!"), __FUNCTION__);
		HandleItemTableLoadRequestFailed(CurrentRequest);
		
		return;
	}

	HandleItemTableLoadRequestSuccess(CurrentRequest);
}

void UTartarusItemSubsystem::HandleItemTableLoadRequestSuccess(const FLoadItemTableRequestInfo* const SuccessRequest)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnLoadItemTableRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), ItemDataTable.Get());
	LoadItemTableRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusItemSubsystem::HandleItemTableLoadRequestFailed(const FLoadItemTableRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnLoadItemTableRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	LoadItemTableRequests.RemoveSingleSwap(*FailedRequest);
}
#pragma endregion

#pragma endregion