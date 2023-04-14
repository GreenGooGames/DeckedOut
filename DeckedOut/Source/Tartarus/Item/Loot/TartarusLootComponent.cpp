// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Loot/TartarusLootComponent.h"

#include "Item/Loot/TartarusLootTableDataAsset.h"
#include "Item/System/TartarusItemSubsystem.h"
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

#pragma region AsyncLoading
FGuid UTartarusLootComponent::AsyncRequestDropLoot(const FTransform& SpawnTransform, const FDropLootRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusLootComponent::HandleLootTableLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(LootTable.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Setup the request for this spawn.
	FLootDropRequestInfo LootDropRequest = FLootDropRequestInfo(OnRequestCompletedEvent, SpawnTransform);
	LootDropRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	LootDropRequests.Add(LootDropRequest);

	return LootDropRequest.GetRequestId();
}

void UTartarusLootComponent::HandleRequestSuccess(const FLootDropRequestInfo* const SuccessRequest, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot)
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

	TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot;

	FailedRequest->OnDropLootRequestCompleted().Broadcast(FailedRequest->GetRequestId(), SpawnedLoot);
	LootDropRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusLootComponent::HandleLootTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FLootDropRequestInfo* const CurrentRequest = LootDropRequests.FindByPredicate([&ASyncLoadRequestId](const FLootDropRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	// Get a reference to the DataTable that got loaded.
	LootTable = Cast<UTartarusLootTableDataAsset>(AssetHandle.Get()->GetLoadedAsset());

	if (!IsValid(LootTable.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: LootTable was not loaded!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	TArray<UTartarusItem*> LootHandles = LootTable->GetLoot();
	
	// Load the item blueprint.
	FGuid LootLoadRequestId = AsyncRequestSpawnItems(LootHandles, CurrentRequest->GetSpawnTransform());
	if (!LootLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not load the item async!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	CurrentRequest->SetASyncLoadRequestId(LootLoadRequestId);
}

FGuid UTartarusLootComponent::AsyncRequestSpawnItems(TArray<UTartarusItem*> ItemHandles, const FTransform& SpawnTransform)
{
	// Get the ItemSpawner.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: ItemSubsystem was invalid!"), *FString(__FUNCTION__));
		
		return FGuid();
	}
	
	FItemSpawnRequestCompletedEvent OnSpawnRequestCompleted;
	OnSpawnRequestCompleted.AddUObject(this, &UTartarusLootComponent::HandleLootSpawned);
	
	FString ContextString;
	const FGuid SpawnRequestId = ItemSubsystem->AsyncRequestSpawnItems(ItemHandles, SpawnTransform, OnSpawnRequestCompleted);
	
	if (!SpawnRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not start a spawn request!"), *FString(__FUNCTION__));
		
		return FGuid();
	}
	
	return SpawnRequestId;
}

void UTartarusLootComponent::HandleLootSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot)
{
	// Get the request that is being handled.
	FLootDropRequestInfo* const CurrentRequest = LootDropRequests.FindByPredicate([&ASyncLoadRequestId](const FLootDropRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	if (SpawnedLoot.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Loot failed: No loot was spawned!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	HandleRequestSuccess(CurrentRequest, SpawnedLoot);
}
#pragma endregion
