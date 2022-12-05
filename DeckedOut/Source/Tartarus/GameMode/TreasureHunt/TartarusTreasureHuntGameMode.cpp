// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Ruleset/TartarusRuleset.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"


void ATartarusTreasureHuntGameMode::StartTreasureHunt()
{
	// Gift the player starter items.
	const AController* const PlayerController = GetWorld()->GetFirstPlayerController<AController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	const bool bHasGiftedItems = GiftStarterItems(PlayerController);
	if (!bHasGiftedItems)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Treasure hunt did not start: Failed to gift starter items!"), *FString(__FUNCTION__));
		return; // Items could not be gifted so don't start a treasure hunt as the player wont be able to complete it.
	}

	// Change the game state to indicate the treasure hunt has started.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);
	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Active);

	// Enable the ruleset for the treasure hunt.
	EnableRuleset();
}

void ATartarusTreasureHuntGameMode::StopTreasureHunt()
{
	// Disable the ruleset.
	DisableRuleset();

	// Change the game state to indicate the treasure hunt has ended.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);
	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Inactive);

	// Remove all gifted items from the player inventory.
	const AController* const PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove gifted items: No player found."), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove gifted items: No inventory found.!"), *FString(__FUNCTION__));
		return;
	}

	Inventory->RetrieveItem(GiftedTreasureKeyInventoryId, GiftTreasureKeyStackCount);
}

#pragma region StarterGifts
bool ATartarusTreasureHuntGameMode::GiftStarterItems(const AController* const PlayerController)
{
	// Get the player ivnentory.
	UTartarusInventoryComponent* const Inventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: No inventory found.!"), *FString(__FUNCTION__));
		return false;
	}

	FString ContextString = "";
	FItemTableRow* const ItemRow = StarterTreasureKeyHandle.GetRow<FItemTableRow>(ContextString);

	if (!ItemRow)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not retrieve the item row!"), *FString(__FUNCTION__));
		return false;
	}

	// TreasureKeys are unique, a StackCount > 1 will result in failure. (If multiple uniques need to be gifted, split them up in an array)
	GiftedTreasureKeyInventoryId = Inventory->StoreItem(ItemRow->UniqueItemId, GiftTreasureKeyStackCount);
	if (!GiftedTreasureKeyInventoryId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	return true;
}
#pragma endregion

#pragma region Ruleset
void ATartarusTreasureHuntGameMode::HandleClankLevelChanged(int32 ClankLevel)
{
	const bool bStageChanged = Ruleset->ActivateStage(GetWorld(), ActiveClankStage, ClankLevel);

	if (!bStageChanged)
	{
		return;
	}
}

void ATartarusTreasureHuntGameMode::EnableRuleset()
{
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);
	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	HandleClankLevelChangedDelegateHandle = TreasureHuntGameState->OnClankLevelChanged().AddUObject(this, &ATartarusTreasureHuntGameMode::HandleClankLevelChanged);
	HandleClankLevelChanged(0);
}

void ATartarusTreasureHuntGameMode::DisableRuleset()
{
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);
	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->OnClankLevelChanged().Remove(HandleClankLevelChangedDelegateHandle);

	Ruleset->Reset(GetWorld());
	ActiveClankStage = INDEX_NONE;
}
#pragma endregion
