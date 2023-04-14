// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusTreasureSubsystem.generated.h"

class ATartarusCompass; 
class ATartarusTreasureChest;

enum class ETreasureHuntState : uint8;

struct FStreamableHandle;

DECLARE_EVENT_OneParam(UTartarusTreasureSubsystem, FSpawnAndLinkRequestCompletedEvent, bool /*bIsSuccess*/);

USTRUCT()
struct FSpawnPointData
{
	GENERATED_BODY()

public:
	FSpawnPointData() {}
	FSpawnPointData(const FTransform& SpawnPointTransform);

	void Reset();
	void Reserve(const FInventoryStackId& KeyInventoryId);
	bool IsAvailable() { return bIsAvailable; }

	const FTransform& GetTransform() const { return Transform; }
	const ATartarusTreasureChest* GetTreasure() const { return Treasure.Get(); }
	void SetTreasure(ATartarusTreasureChest* TreasureChest) { Treasure = TreasureChest; }
	const FInventoryStackId GetKeyId() const { return KeyInventoryStackId; }

private:
	bool bIsAvailable = true;

	FTransform Transform = FTransform();
	TObjectPtr<ATartarusTreasureChest> Treasure = nullptr;
	FInventoryStackId KeyInventoryStackId = FInventoryStackId();
};

#pragma region AsyncLoading
USTRUCT()
struct FTreasureSpawnRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FTreasureSpawnRequestInfo() {}
	FTreasureSpawnRequestInfo(FSpawnPointData* const SpawnPoint, const FInventoryStackId ToLinkKeyInventoryStackId, FSpawnAndLinkRequestCompletedEvent& OnRequestCompleted)
	{
		RequestId = FGuid::NewGuid();

		RequestCompletedEvent = OnRequestCompleted;
		SpawnPointData = SpawnPoint;
	}

	const FSpawnAndLinkRequestCompletedEvent& OnRequestCompleted() const { return RequestCompletedEvent; }
	FSpawnPointData& GetSpawnPointData() const { return *SpawnPointData; }
	FInventoryStackId GetKeyInventoryStackId() { return SpawnPointData->GetKeyId(); }

private:
	FSpawnAndLinkRequestCompletedEvent RequestCompletedEvent = FSpawnAndLinkRequestCompletedEvent();
	
	FSpawnPointData* SpawnPointData = nullptr;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusTreasureSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UTartarusTreasureSubsystem();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	/*
	* Retrieves the location of treasure that is linked to the key.
	* Return: The location of the Treasure in the world.
	*/
	FVector GetTreasureLocation(const FInventoryStackId& KeyInventoryId) const;

	/*
	* Retrieves the key used to open the given treasure.
	* Return: the InventoryStackId of the key.
	*/
	FInventoryStackId GetTreasureKey(const ATartarusTreasureChest* const Treasure);

protected:
	/*
	* Spawns a treasure in the world.
	* Return: The Treasure that is spawned, nullptr if spawn failed.
	*/
	ATartarusTreasureChest* SpawnTreasure(TSubclassOf<ATartarusTreasureChest>& TreasureClass, FSpawnPointData& SpawnPointData);

	// Fired when a treasure is looted, called by the treasure itself.
	void HandleOnTreasureLooted(ATartarusTreasureChest* const LootedTreasure);

	// Fired when the GameState changes, Spawns/Despawns all treasures.
	void HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState);

	// Fired when all of the treasure keys their data has been received.
	void HandleTreasureKeysDataReceived(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> TreasureKeysData);

#pragma region SpawnPoint
public:
	// Register a trasnform to be used as a location to spawn a treasure chest.
	void RegisterSpawnTransform(const FTransform& SpawnTransform);

protected:
	FSpawnPointData* FindAvailableSpawnpoint();

private:
	TArray<FSpawnPointData> SpawnPoints;
#pragma endregion

#pragma region AsyncLoading
public:
	/*
	Create a request to spawn a treasure chest and link a key to it.
	Return: The unique id of this request.
	*/
	FGuid AsyncRequestSpawnAndLink(const FInventoryStackId KeyInventoryStackId, FSpawnAndLinkRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FTreasureSpawnRequestInfo* const SuccessRequest);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FTreasureSpawnRequestInfo* const FailedRequest);

	// Called when the AssetManager finishes loading the TreasureClass.
	void HandleTreasureClassLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

	// Cancels all active Async requests.
	void CancelASyncRequests();

private:
	TArray<FTreasureSpawnRequestInfo> SpawnAndLinkRequests;
	TSoftClassPtr<ATartarusTreasureChest> TreasureClass = nullptr;
#pragma endregion
};
