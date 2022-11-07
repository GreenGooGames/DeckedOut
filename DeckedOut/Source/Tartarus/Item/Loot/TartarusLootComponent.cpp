// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Loot/TartarusLootComponent.h"

#include "Engine/DataTable.h"
#include "Item/Loot/TartarusLootTableData.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItemBase.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"

// Sets default values for this component's properties
UTartarusLootComponent::UTartarusLootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

const FLootTableRow* UTartarusLootComponent::GetRandomLootEntry() const
{
	// Retrieve all rows in the datatable.
	FString ContextString = "";
	TArray<FLootTableRow*> DataTableRows;

	LootTable->GetAllRows<FLootTableRow>(ContextString, DataTableRows);

	if (DataTableRows.IsEmpty())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: No entries in the datatable.!"), __FUNCTION__);
		return nullptr;
	}

	// Pick a random Row to drop.
	const int32 Index = FMath::RandRange(0, DataTableRows.Num() - 1);
	const FLootTableRow* const LootDefinition = DataTableRows[Index];

	if (!LootDefinition)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: LootDefinition is invalid!"), __FUNCTION__);
		return nullptr;
	}

	return LootDefinition;
}

#pragma region AsyncLoading
FGuid UTartarusLootComponent::AsyncRequestDropLoot(const FTransform& SpawnTransform, const FDropLootRequestCompletedEvent& OnRequestCompletedEvent)
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
	OnRequestCompleted.AddUObject(this, &UTartarusLootComponent::HandleDataTableLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(LootTable.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	// Setup the request for this spawn.
	FLootDropRequestInfo LootDropRequest = FLootDropRequestInfo(OnRequestCompletedEvent, SpawnTransform);
	LootDropRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	LootDropRequests.Add(LootDropRequest);

	return LootDropRequest.GetRequestId();
}

void UTartarusLootComponent::HandleRequestSuccess(const FLootDropRequestInfo* const SuccessRequest, TWeakObjectPtr<ATartarusItemBase> SpawnedLoot)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnDropLootRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), SpawnedLoot);
	LootDropRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusLootComponent::HandleRequestFailed(const FLootDropRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnDropLootRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	LootDropRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusLootComponent::HandleDataTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FLootDropRequestInfo* const CurrentRequest = LootDropRequests.FindByPredicate([&ASyncLoadRequestId](const FLootDropRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	// Get a reference to the DataTable that got loaded.
	LootTable = Cast<UDataTable>(AssetHandle.Get()->GetLoadedAsset());

	if (!IsValid(LootTable.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: datatable was not loaded!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	// [Koen Goossens] TODO: Add support to drop multiple items instead of 1 random entry.
	// Load the item blueprint.
	FGuid LootLoadRequestId = AsyncRequestSpawnItem(GetRandomLootEntry(), CurrentRequest->GetSpawnTransform());

	if (!LootLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not load the item async!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	CurrentRequest->SetASyncLoadRequestId(LootLoadRequestId);
}

FGuid UTartarusLootComponent::AsyncRequestSpawnItem(const FLootTableRow* const LootDefinition, const FTransform& SpawnTransform)
{
	// Get the ItemSpawner.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();

	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: ItemSubsystem was invalid!"), __FUNCTION__);
		
		return FGuid();
	}

	FItemSpawnRequestCompletedEvent OnSpawnRequestCompleted;
	OnSpawnRequestCompleted.AddUObject(this, &UTartarusLootComponent::HandleLootSpawned);

	const FGuid SpawnRequestId = ItemSubsystem->ASyncRequestSpawnItem(LootDefinition->ItemId, SpawnTransform, OnSpawnRequestCompleted);

	if (!SpawnRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not start a spawn request!"), __FUNCTION__);
		
		return FGuid();
	}

	return SpawnRequestId;
}

void UTartarusLootComponent::HandleLootSpawned(FGuid ASyncLoadRequestId, TWeakObjectPtr<ATartarusItemBase> SpawnedLoot)
{
	// Get the request that is being handled.
	FLootDropRequestInfo* const CurrentRequest = LootDropRequests.FindByPredicate([&ASyncLoadRequestId](const FLootDropRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	if (!IsValid(SpawnedLoot.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: No loot was spawned!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	HandleRequestSuccess(CurrentRequest, SpawnedLoot);
}
#pragma endregion
