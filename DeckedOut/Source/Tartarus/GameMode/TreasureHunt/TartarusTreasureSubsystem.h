// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusTreasureSubsystem.generated.h"

class ATartarusCompass; 
class ATartarusTreasureChest;

struct FStreamableHandle;

DECLARE_EVENT_OneParam(UTartarusTreasureSubsystem, FSpawnAndLinkRequestCompletedEvent, bool /*bIsSuccess*/);

USTRUCT()
struct FSpawnPointData
{
	GENERATED_BODY()

	bool bIsAvailable = true;

	FTransform Transform = FTransform();
	TWeakObjectPtr<ATartarusTreasureChest> Treasure;
};

#pragma region AsyncLoading
USTRUCT()
struct FTreasureSpawnRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FTreasureSpawnRequestInfo() {}
	FTreasureSpawnRequestInfo(FSpawnPointData* const SpawnPoint, const FGuid ToLinkKeyInventoryStackId, FSpawnAndLinkRequestCompletedEvent& OnRequestCompleted)
	{
		RequestId = FGuid::NewGuid();

		RequestCompletedEvent = OnRequestCompleted;
		SpawnPointData = SpawnPoint;
		KeyInventoryStackId = ToLinkKeyInventoryStackId;
	}

	const FSpawnAndLinkRequestCompletedEvent& OnRequestCompleted() const { return RequestCompletedEvent; }
	FSpawnPointData& GetSpawnPointData() const { return *SpawnPointData; }
	FGuid GetKeyInventoryStackId() { return KeyInventoryStackId; }

private:
	FSpawnAndLinkRequestCompletedEvent RequestCompletedEvent = FSpawnAndLinkRequestCompletedEvent();
	
	FSpawnPointData* SpawnPointData = nullptr;
	FGuid KeyInventoryStackId = FGuid();
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

protected:
	/*
	* Spawns a treasure in the world.
	* Return: The Treasure that is spawned, nullptr if spawn failed.
	*/
	ATartarusTreasureChest* SpawnTreasure(TSubclassOf<ATartarusTreasureChest>& TreasureClass, const FTransform& SpawnTransform);

	/* 
	* Links a compass and a treasure to eachother.
	* Return: True if linked, false if link failed.
	*/
	bool LinkKeyToTreasure(const FGuid KeyInventoryStackId, TObjectPtr<ATartarusTreasureChest> Treasure);

	// Fired when a treasure is looted, called by the treasure itself.
	UFUNCTION()
	void HandleOnTreasureLooted(ATartarusTreasureChest* const LootedTreasure);
	
	// [Koen Goossens] TODO: Add public method to get treasure location by using a key inventory id.

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
	FGuid AsyncRequestSpawnAndLink(const FGuid KeyInventoryStackId, FSpawnAndLinkRequestCompletedEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FTreasureSpawnRequestInfo* const SuccessRequest);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FTreasureSpawnRequestInfo* const FailedRequest);


	// Called when the AssetManager finishes loading the TreasureClass.
	void HandleTreasureClassLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

private:
	TArray<FTreasureSpawnRequestInfo> SpawnAndLinkRequests;
	TSoftClassPtr<ATartarusTreasureChest> TreasureClass = nullptr;

#pragma endregion
};
