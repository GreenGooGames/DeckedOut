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
FGuid UTartarusItemSubsystem::ASyncRequestSpawnItem(const int32 ItemId, const FTransform& SpawnTransform, FItemSpawnRequestCompletedEvent& OnRequestCompletedEvent)
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
	OnRequestCompleted.AddUObject(this, &UTartarusItemSubsystem::HandleDataTableLoaded);
	
	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDataTable.ToSoftObjectPath(), OnRequestCompleted);
	
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	// Setup the request for this spawn.
	FItemSpawnRequestInfo SpawnRequest = FItemSpawnRequestInfo(OnRequestCompletedEvent, ItemId, SpawnTransform);
	SpawnRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	SpawnRequests.Add(SpawnRequest);

	return SpawnRequest.GetRequestId();
}

void UTartarusItemSubsystem::HandleRequestSuccess(const FItemSpawnRequestInfo* const SuccessRequest, TWeakObjectPtr<ATartarusItemBase> SpawnedItem)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnItemSpawnRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), SpawnedItem);
	SpawnRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusItemSubsystem::HandleRequestFailed(const FItemSpawnRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnItemSpawnRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	SpawnRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusItemSubsystem::HandleDataTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
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
	
	// Get a reference to the DataTable that got loaded.
	UDataTable* const Datatable = Cast<UDataTable>(AssetHandle.Get()->GetLoadedAsset());
	
	if (!IsValid(Datatable))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: datatable was not loaded!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}
	
	// Get the Item that was requestes from the DataTable
	FItemTableRow* const ItemDefinition = FindItemRow(Datatable, CurrentRequest->GetItemId());
	
	if (!ItemDefinition)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Item was not found in the DataTable!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}
	
	// Load the Item asynchronously.
	FGuid ItemLoadRequestId = AsyncRequestLoadItem(ItemDefinition);

	if (!ItemLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Could not load the item async!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);
		
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
	OnRequestCompleted.AddUObject(this, &UTartarusItemSubsystem::HandleBlueprintLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDefinition->Blueprint.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	return AsyncLoadRequestId;
}

void UTartarusItemSubsystem::HandleBlueprintLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
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
		HandleRequestFailed(CurrentRequest);
		
		return;
	}

	ATartarusItemBase* const SpawnedItem = SpawnItem(ItemClass, CurrentRequest->GetSpawnTransform());

	if (!IsValid(SpawnedItem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Item failed: Item failed to spawn!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	SpawnedItem->SetReferenceId(CurrentRequest->GetItemId());

	HandleRequestSuccess(CurrentRequest, SpawnedItem);
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