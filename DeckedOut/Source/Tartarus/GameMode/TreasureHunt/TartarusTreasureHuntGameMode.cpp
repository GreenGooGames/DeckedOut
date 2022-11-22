// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"
#include "Item/Equipable/TartarusEquipableManager.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"

void ATartarusTreasureHuntGameMode::StartTreasureHunt()
{
	// Change the game state to indicate the treasure hunt has started.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);

	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Active);

	// Gift all players a compass and spawn a linked chest in the world.
	for (APlayerState* const PlayerState : TreasureHuntGameState->PlayerArray)
	{
		AController* PlayerController = PlayerState->GetOwningController();
		GiftStarterItems(PlayerController);
	}
}

void ATartarusTreasureHuntGameMode::StopTreasureHunt()
{
	// Change the game state to indicate the treasure hunt has ended.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);

	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Inactive);
}

void ATartarusTreasureHuntGameMode::GiftStarterItems(AController* const PlayerController) const
{
	// Get the player ivnentory.
	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		return;
	}

	FString ContextString = "";
	FItemTableRow* const ItemRow = StarterCompass.GetRow<FItemTableRow>(ContextString);

	if (!ItemRow)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not retrieve the item row!"), __FUNCTION__);
		return;
	}

	const FGuid StackId = Inventory->StoreItem(ItemRow->UniqueItemId, NumGiftCompasses);

	// [Koen Goossens] TODO: If the compass cannot be gifted, then the interaction should fail and the door should not open.
	if (!StackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the inventory!"), __FUNCTION__);
		return;
	}

	// Spawn a chest for this compass.
	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();

	FSpawnAndLinkRequestCompletedEvent OnRequestCompleted;
	TreasureSubsystem->AsyncRequestSpawnAndLink(StackId, OnRequestCompleted);

	// Auto-equip the compass.
	UTartarusEquipableManager* const EquipableManager = PlayerController->GetPawn()->FindComponentByClass<UTartarusEquipableManager>();

	if (!IsValid(EquipableManager))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not auto-equip the gift item: No equipableManager found!"), __FUNCTION__);
		return;
	}

	bool bIsTryingToEquip = EquipableManager->ASyncRequestEquip(StackId, StaticCast<EEquipmentSlot>(AutoEquipCompassSlotMask));
}
