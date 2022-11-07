// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"

#include "GameMode/TreasureHunt/TartarusTreasureSubsystemSettings.h"
#include "Item/Equipable/Equipment/TartarusCompass.h"
#include "Item/Persistent/TartarusTreasureChest.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"

UTartarusTreasureSubsystem::UTartarusTreasureSubsystem()
{
	const UTartarusTreasureSubsystemSetting* const Settings = GetDefault<UTartarusTreasureSubsystemSetting>();

	if (IsValid(Settings))
	{
		TreasureClass = Settings->TreasureClass;
	}

}

ATartarusTreasureChest* UTartarusTreasureSubsystem::SpawnTreasure(TSubclassOf<ATartarusTreasureChest>& ToSpawnClass, const FTransform& SpawnTransform)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	// Spawn the treasure in the world.
	ATartarusTreasureChest* const Treasure = GetWorld()->SpawnActor<ATartarusTreasureChest>(ToSpawnClass, SpawnTransform, SpawnParams);
	
	if (!Treasure)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn the treasure!"), __FUNCTION__);
		return nullptr;
	}

	Treasure->OnLooted.AddDynamic(this, &UTartarusTreasureSubsystem::HandleOnTreasureLooted);

	return Treasure;
}

bool UTartarusTreasureSubsystem::LinkCompassToTreasure(TWeakObjectPtr<ATartarusCompass> Compass, TObjectPtr<ATartarusTreasureChest> Treasure)
{
	if (!IsValid(Compass.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to link a compass to treasure: Compass was invalid!"), __FUNCTION__);
		return false;
	}

	if (!IsValid(Treasure))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to link a compass to treasure: Treasure was invalid!"), __FUNCTION__);
		return false;
	}

	Treasure->LinkCompass(Compass);
	Compass->SetTargetLocation(Treasure->GetActorLocation());

	return true;
}

void UTartarusTreasureSubsystem::HandleOnTreasureLooted(ATartarusTreasureChest* const LootedTreasure)
{
	if (!IsValid(LootedTreasure))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: The treasure is invalid!"), __FUNCTION__);
		return;
	}

	for (FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint.Treasure != LootedTreasure)
		{
			continue;
		}

		SpawnPoint.bIsAvailable = true;
		SpawnPoint.Treasure = nullptr;

		LootedTreasure->Destroy();
	}
}

#pragma region SpawnPoint
void UTartarusTreasureSubsystem::RegisterSpawnTransform(const FTransform& SpawnTransform)
{
	FSpawnPointData Data = FSpawnPointData();
	Data.Transform = SpawnTransform;

	SpawnPoints.Add(Data);
}

FSpawnPointData* UTartarusTreasureSubsystem::FindAvailableSpawnpoint()
{
	TArray<FSpawnPointData*> AvailablePoints;

	for (FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		// If the Spawnpoint is already being used, skip.
		if (!SpawnPoint.bIsAvailable)
		{
			continue;
		}

		AvailablePoints.Add(&SpawnPoint);
	}

	if (AvailablePoints.IsEmpty())
	{
		return nullptr;
	}

	return AvailablePoints[FMath::RandRange(0, AvailablePoints.Num() - 1)];
}

#pragma endregion

#pragma region AsyncLoading
FGuid UTartarusTreasureSubsystem::AsyncRequestSpawnedAndLinkTreasure(TWeakObjectPtr<ATartarusCompass> Compass, FSpawnAndLinkRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Is the compass currently valid?
	if (!IsValid(Compass.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: The compass is invalid!"), __FUNCTION__);

		return FGuid();
	}
	
	// Get an available spawn transform.
	FSpawnPointData* const SpawnPointData = FindAvailableSpawnpoint();
	
	if (!SpawnPointData)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: No valid spawn point found!"), __FUNCTION__);
		
		return FGuid();
	}
	
	// Async load the treasure class.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: Asset manager was invalid!"), __FUNCTION__);
		return FGuid();
	}
	
	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusTreasureSubsystem::HandleTreasureClassLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(TreasureClass.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}
	
	FTreasureSpawnRequestInfo SpawnRequestInfo = FTreasureSpawnRequestInfo(SpawnPointData, Compass.Get(), OnRequestCompletedEvent);
	SpawnRequestInfo.SetASyncLoadRequestId(AsyncLoadRequestId);
	SpawnRequestInfo.GetSpawnPointData().bIsAvailable = false;

	SpawnAndLinkRequests.Add(SpawnRequestInfo);

	return SpawnRequestInfo.GetRequestId();
}

void UTartarusTreasureSubsystem::HandleRequestSuccess(const FTreasureSpawnRequestInfo* const SuccessRequest)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnRequestCompleted().Broadcast(true);

	SpawnAndLinkRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusTreasureSubsystem::HandleRequestFailed(const FTreasureSpawnRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnRequestCompleted().Broadcast(false);
	FailedRequest->GetSpawnPointData().bIsAvailable = true;

	if (IsValid(FailedRequest->GetSpawnPointData().Treasure.Get()))
	{
		FailedRequest->GetSpawnPointData().Treasure->Destroy();
	}

	SpawnAndLinkRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusTreasureSubsystem::HandleTreasureClassLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FTreasureSpawnRequestInfo* const CurrentRequest = SpawnAndLinkRequests.FindByPredicate([&ASyncLoadRequestId](const FTreasureSpawnRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Treasure failed: Could not find the request!"), __FUNCTION__);

		return;
	}

	TreasureClass = Cast<UClass>(AssetHandle.Get()->GetLoadedAsset());

	if (!IsValid(TreasureClass.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Treasure failed: Class failed to load!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	// Spawn the treasure in the world.
	TSubclassOf<ATartarusTreasureChest> LoadedClass = TreasureClass.Get();
	ATartarusTreasureChest* const Treasure = SpawnTreasure(LoadedClass, CurrentRequest->GetSpawnPointData().Transform);
	
	if (!IsValid(Treasure))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn Treasure: Could not spawn in the world!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);
	
		return;
	}
	
	// Link the treasure to the spawnpoint.
	CurrentRequest->GetSpawnPointData().Treasure = Treasure;

	// Link a compass to the treasure.
	bool bIsLinked = LinkCompassToTreasure(CurrentRequest->GetCompass(), Treasure);
	
	if (!bIsLinked)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn Treasure: Could not establish a link!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}
	
	HandleRequestSuccess(CurrentRequest);
}
#pragma endregion