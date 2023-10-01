// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"

#include "Engine/World.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "GameMode/TreasureHunt/TartarusTreasureSubsystemSettings.h"
#include "Item/Equipable/Equipment/TartarusCompass.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Persistent/TartarusTreasureChest.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"

#pragma region FSpawnPointData
FSpawnPointData::FSpawnPointData(const FTransform& SpawnPointTransform)
{
	Transform = SpawnPointTransform;
}

void FSpawnPointData::Reset()
{
	bIsAvailable = true;
	KeyInventoryStackId.Invalidate();

	if (IsValid(Treasure.Get()))
	{
		Treasure.Get()->Destroy();
		Treasure = nullptr;
	}
}

void FSpawnPointData::Reserve(const FInventoryStackId& KeyInventoryId)
{
	bIsAvailable = false;
	KeyInventoryStackId = KeyInventoryId;
}
#pragma endregion

UTartarusTreasureSubsystem::UTartarusTreasureSubsystem()
{
	const UTartarusTreasureSubsystemSetting* const Settings = GetDefault<UTartarusTreasureSubsystemSetting>();

	if (IsValid(Settings))
	{
		TreasureClass = Settings->TreasureClass;
	}
}

void UTartarusTreasureSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();

	if (!IsValid(GameState))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to subscribe to the game state: No game valid state found!"), *FString(__FUNCTION__));
		return;
	}

	GameState->OnRunningStateChanged().AddUObject(this, &UTartarusTreasureSubsystem::HandleGameRunningStateChanged);
}

ATartarusTreasureChest* UTartarusTreasureSubsystem::SpawnTreasure(TSubclassOf<ATartarusTreasureChest>& ToSpawnClass, FSpawnPointData& SpawnPointData)
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	// Spawn the treasure in the world.
	ATartarusTreasureChest* const Treasure = GetWorld()->SpawnActor<ATartarusTreasureChest>(ToSpawnClass, SpawnPointData.GetTransform(), SpawnParams);
	if (!Treasure)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn the treasure!"), *FString(__FUNCTION__));
		return nullptr;
	}

	Treasure->OnLooted().AddUObject(this, &UTartarusTreasureSubsystem::HandleOnTreasureLooted);

	SpawnPointData.SetTreasure(Treasure);

	return Treasure;
}

void UTartarusTreasureSubsystem::HandleOnTreasureLooted(ATartarusTreasureChest* const LootedTreasure)
{
	if (!IsValid(LootedTreasure))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: The treasure is invalid!"), *FString(__FUNCTION__));
		return;
	}

	for (FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint.GetTreasure() != LootedTreasure)
		{
			continue;
		}

		SpawnPoint.Reset();
	}
}

void UTartarusTreasureSubsystem::HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState)
{
	// Spawn and Link treasure when the treasure hunt starts.
	if (NewState == ETreasureHuntState::Active && OldState == ETreasureHuntState::Inactive)
	{
		// Find all treasure keys in the inventory of the player, by first getting all treasure key id's.
		UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
		if (!IsValid(ItemSubsystem))
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to treasure: Item subsystem is invalid!"), *FString(__FUNCTION__));
			return;
		}

		TArray<EItemType> ItemTypes;
		ItemTypes.Add(EItemType::TreasureKey);
		FGetItemDataRequestCompletedEvent OnRequestCompleted;
		OnRequestCompleted.AddUObject(this, &UTartarusTreasureSubsystem::HandleTreasureKeysDataReceived);

		ItemSubsystem->AsyncRequestGetItemsData(ItemTypes, OnRequestCompleted);
	}
	// Despawn all treasure when the treasure hunt ends.
	else if (NewState == ETreasureHuntState::Inactive)
	{
		// Clear all data associated with spawned treasures.
		for (FSpawnPointData& SpawnPoint : SpawnPoints)
		{
			SpawnPoint.Reset();
		}

		CancelASyncRequests();
	}
}

FVector UTartarusTreasureSubsystem::GetTreasureLocation(const FInventoryStackId& KeyInventoryId) const
{
	if (!KeyInventoryId.IsValid())
	{
		return FTartarusHelpers::InvalidLocation;
	}

	for (const FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint.GetKeyId() != KeyInventoryId)
		{
			continue;
		}

		return SpawnPoint.GetTransform().GetLocation();
	}

	return FTartarusHelpers::InvalidLocation;
}

FInventoryStackId UTartarusTreasureSubsystem::GetTreasureKey(const ATartarusTreasureChest* const Treasure)
{
	if (!IsValid(Treasure))
	{
		return FInventoryStackId();
	}

	for (const FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint.GetTreasure() != Treasure)
		{
			continue;
		}

		return SpawnPoint.GetKeyId();
	}

	return FInventoryStackId();
}

void UTartarusTreasureSubsystem::HandleTreasureKeysDataReceived(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> TreasureKeysData)
{
	// Get the inventory of the player.
	const AController* const PlayerController = GetWorld()->GetFirstPlayerController<AController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to Treasure: No player in the world!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to Treasure: No inventory on player!"), *FString(__FUNCTION__));
		return;
	}

	// Loop over each treasure Key, and look if the player has it in their inventory.
	for (const UTartarusItem* const TreasureKey : TreasureKeysData)
	{
		const TArray<const FInventoryStack*> InventoryTreasureKeys = Inventory->GetOverviewMulti(TreasureKey->InventoryId, TreasureKey->GetPrimaryAssetId());
	
		for (const FInventoryStack* const ItemStack : InventoryTreasureKeys)
		{
			// Spawn a treasure chest and link it to the treasure key.
			FSpawnAndLinkRequestCompletedEvent OnRequestCompleted;
			AsyncRequestSpawnAndLink(ItemStack->GetStackId(), OnRequestCompleted);
		}
	}
}

#pragma region SpawnPoint
void UTartarusTreasureSubsystem::RegisterSpawnTransform(const FTransform& SpawnTransform)
{
	const FSpawnPointData SpawnPoint = FSpawnPointData(SpawnTransform);

	SpawnPoints.Add(SpawnPoint);
}

FSpawnPointData* UTartarusTreasureSubsystem::FindAvailableSpawnpoint()
{
	TArray<FSpawnPointData*> AvailablePoints;

	for (FSpawnPointData& SpawnPoint : SpawnPoints)
	{
		// If the Spawnpoint is already being used, skip.
		if (!SpawnPoint.IsAvailable())
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
FGuid UTartarusTreasureSubsystem::AsyncRequestSpawnAndLink(const FInventoryStackId KeyInventoryStackId, FSpawnAndLinkRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Is the Key valid?
	if (!KeyInventoryStackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: The Key is invalid!"), *FString(__FUNCTION__));

		return FGuid();
	}
	
	// Get an available spawn transform.
	FSpawnPointData* const SpawnPointData = FindAvailableSpawnpoint();
	if (!SpawnPointData)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: No valid spawn point found!"), *FString(__FUNCTION__));
		
		return FGuid();
	}
	
	// Async load the treasure class.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsInitialized())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request failed: Asset manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusTreasureSubsystem::HandleTreasureClassLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(TreasureClass.ToSoftObjectPath(), OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	FTreasureSpawnRequestInfo SpawnRequestInfo = FTreasureSpawnRequestInfo(SpawnPointData, KeyInventoryStackId, OnRequestCompletedEvent);
	SpawnRequestInfo.SetASyncLoadRequestId(AsyncLoadRequestId);
	SpawnRequestInfo.GetSpawnPointData().Reserve(KeyInventoryStackId);
	
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
	FailedRequest->GetSpawnPointData().Reset();

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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Treasure failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	// Check for a succesfull load.
	TreasureClass = Cast<UClass>(AssetHandle.Get()->GetLoadedAsset());
	if (!IsValid(TreasureClass.Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Spawn Treasure failed: Class failed to load!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	// Check the player inventory for the key to link to this treasure.
	AController* const PlayerController = GetWorld()->GetFirstPlayerController<AController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to link key to Treasure: No player in the world!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to link key to Treasure: No inventory on player!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	if (!Inventory->Contains(CurrentRequest->GetKeyInventoryStackId()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to link key to Treasure: Inventory doesn't have the key!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	// Spawn the treasure in the world.
	TSubclassOf<ATartarusTreasureChest> LoadedClass = TreasureClass.Get();
	ATartarusTreasureChest* const Treasure = SpawnTreasure(LoadedClass, CurrentRequest->GetSpawnPointData());
	if (!IsValid(Treasure))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to spawn Treasure: Could not spawn in the world!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	HandleRequestSuccess(CurrentRequest);
}

void UTartarusTreasureSubsystem::CancelASyncRequests()
{
	// Clear all ongoing AsyncRequests.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (AssetManager.IsInitialized())
	{
		for (const FTreasureSpawnRequestInfo& AsyncRequest : SpawnAndLinkRequests)
		{
			AssetManager.CancelRequest(AsyncRequest.GetASyncLoadRequestId());

			// Notify any listeners that the request got canceled (/failed).
			HandleRequestFailed(&AsyncRequest);
		}
	}

	SpawnAndLinkRequests.Empty();
}
#pragma endregion