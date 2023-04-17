// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusLootComponent.generated.h"

class ATartarusItemInstance;
class UTartarusLootTableDataAsset;
class UTartarusItem;

struct FDataTableRowHandle;
struct FStreamableHandle;

DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FDropLootRequestCompletedEvent, FGuid /*RequestId*/, TArray<TWeakObjectPtr<ATartarusItemInstance>> /*SpawnedItems*/)

USTRUCT()
struct FLootDropRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FLootDropRequestInfo() {};
	FLootDropRequestInfo(const FDropLootRequestCompletedEvent& OnCompleted, const FTransform& ItemSpawnTransform)
	{
		RequestId = FGuid::NewGuid();

		RequestCompleteEvent = OnCompleted;
		SpawnTransform = ItemSpawnTransform;
	}

	const FDropLootRequestCompletedEvent& OnDropLootRequestCompleted() const { return RequestCompleteEvent; }
	const FTransform& GetSpawnTransform() const { return SpawnTransform; }

private:
	FTransform SpawnTransform = FTransform();
	FDropLootRequestCompletedEvent RequestCompleteEvent;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTartarusLootComponent();

protected:
	// Table that contains the loot to be dropped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<UTartarusLootTableDataAsset> LootTable = nullptr;

#pragma region AsyncLoading
public:
	/*
	* Creates a request to spawn loot in the world.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestDropLoot(const FTransform& SpawnTransform, const FDropLootRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FLootDropRequestInfo* const SuccessRequest, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FLootDropRequestInfo* const FailedRequest);

	// Called when the loot datatable is loaded.
	void HandleLootTableLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

	/*
	* Creates a request to load an item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestSpawnItems(TArray<UTartarusItem*> ItemHandles, const FTransform& SpawnTransform);

	// Called when the item blueprint is loaded.
	void HandleLootSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot);

private:
	TArray<FLootDropRequestInfo> LootDropRequests;

#pragma endregion
};
