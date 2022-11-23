// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Item/Equipable/TartarusEquipableManager.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"

void ATartarusTreasureHuntGameMode::StartTreasureHunt() const
{
	// Change the game state to indicate the treasure hunt has started.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);

	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	// Gift the player starter items.
	const AController* const PlayerController = GetWorld()->GetFirstPlayerController<AController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	const bool bHasGiftedItems = GiftStarterItems(PlayerController);
	if (!bHasGiftedItems)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Treasure hunt did not start: Failed to gift starter items!"), __FUNCTION__);
		return; // Items could not be gifted so don't start a treasure hunt as the player wont be able to complete it.
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Active);
}

void ATartarusTreasureHuntGameMode::StopTreasureHunt() const
{
	// Change the game state to indicate the treasure hunt has ended.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);

	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Inactive);
}

bool ATartarusTreasureHuntGameMode::GiftStarterItems(const AController* const PlayerController) const
{
	// Get the player ivnentory.
	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: No inventory found.!"), __FUNCTION__);
		return false;
	}

	FString ContextString = "";
	FItemTableRow* const ItemRow = StarterCompass.GetRow<FItemTableRow>(ContextString);

	if (!ItemRow)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not retrieve the item row!"), __FUNCTION__);
		return false;
	}

	const FGuid StackId = Inventory->StoreItem(ItemRow->UniqueItemId, NumGiftCompasses);

	// [Koen Goossens] TODO: If the compass cannot be gifted, then the interaction should fail and the door should not open.
	if (!StackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the inventory!"), __FUNCTION__);
		return false;
	}

	// Optional: Auto-equip the compass.
	UTartarusEquipableManager* const EquipableManager = PlayerController->GetPawn()->FindComponentByClass<UTartarusEquipableManager>();

	if (!IsValid(EquipableManager))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not auto-equip the gift item: No EquipableManager found!"), __FUNCTION__);
	}
	else
	{
		bool bIsTryingToEquip = EquipableManager->ASyncRequestEquip(StackId, StaticCast<EEquipmentSlot>(AutoEquipCompassSlotMask));
	}

	return true;
}
