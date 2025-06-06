// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"

#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Ruleset/TartarusRuleSubsystem.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "Inventory/CorrbolgInventoryManagerComponent.h"

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

	// Apply the ruleset for the treasure hunt.
	if (!ApplyRuleset())
	{
		return; // Ruleset could not be applied, nothing will happen during the treasure hunt.
	}

	// Change the game state to indicate the treasure hunt has started.
	ATartarusTreasureHuntGameState* const TreasureHuntGameState = Cast<ATartarusTreasureHuntGameState>(GameState);
	if (!IsValid(TreasureHuntGameState))
	{
		return;
	}

	TreasureHuntGameState->ChangeTreasureHuntState(ETreasureHuntState::Active);
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

	// Remove all gifted items from the player inventory.
	const AController* const PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove gifted items: No player found."), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const TartarusInventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	UCorrbolgInventoryManagerComponent* const Inventory = PlayerController->FindComponentByClass<UCorrbolgInventoryManagerComponent>();
	if (!IsValid(TartarusInventory) || !IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove gifted items: No inventory found.!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusItem* const GiftItem = Cast<UTartarusItem>(TreaseKeyGift.LoadSynchronous());
	if (!IsValid(GiftItem))
	{
		return;
	}

	TartarusInventory->RetrieveEntry(GiftedTreasureKeyInventoryId, 1);
	Inventory->RetrieveEntry(GiftItem->Id);
}

#pragma region StarterGifts
bool ATartarusTreasureHuntGameMode::GiftStarterItems(const AController* const PlayerController)
{
	// Get the player inventory.
	UTartarusInventoryComponent* const InstigatorTartarusInventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	UCorrbolgInventoryManagerComponent* const InstigatorInventory = PlayerController->FindComponentByClass<UCorrbolgInventoryManagerComponent>();
	if (!IsValid(InstigatorTartarusInventory) || !IsValid(InstigatorInventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: No inventory found.!"), *FString(__FUNCTION__));
		return false;
	}

	// TODO: Load the Primary Asset Async.
	UTartarusItem* const GiftItem = Cast<UTartarusItem>(TreaseKeyGift.LoadSynchronous());
	if (!IsValid(GiftItem))
	{
		return false;
	}

	GiftedTreasureKeyInventoryId = InstigatorTartarusInventory->StoreEntry(GiftItem, 1);
	if (!GiftedTreasureKeyInventoryId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the Tartarus inventory!"), *FString(__FUNCTION__));
		return false;
	}

	TObjectPtr<UCorrbolgInventoryEntryDefinition> EntryDefinition = GiftItem->CreateEntryDefinition();
	const bool bIsStored = InstigatorInventory->StoreEntry(EntryDefinition.Get());
	if (!bIsStored)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	return true;
}
#pragma endregion

#pragma region RuleSet
// Applies the ruleset to the Rule subsystem.
bool ATartarusTreasureHuntGameMode::ApplyRuleset() const
{
	const UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	UTartarusRuleSubsystem* const RuleSubsystem = World->GetSubsystem<UTartarusRuleSubsystem>();
	if (!IsValid(RuleSubsystem))
	{
		return false;
	}

	RuleSubsystem->SetRuleset(RulesetData);

	return true;
}
#pragma endregion
