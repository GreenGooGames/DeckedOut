// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContextAction/TartarusEquipContextAction.h"

#include "Item/Equipable/TartarusEquipableManager.h"
#include "Logging/TartarusLogChannels.h"
#include "UI/ContextAction/TartarusContextMenuWidget.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusEquipContextAction::ExecuteAction()
{
	// Get the equipable manager on the player.
	const APlayerController* const PlayerController = ParentMenu->GetOwningPlayer<APlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Construct inventory view failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	const APawn* const PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Construct inventory view failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusEquipableManager* const EquipableManager = PlayerPawn->FindComponentByClass<UTartarusEquipableManager>();
	if (!IsValid(EquipableManager))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Construct inventory view failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	// Try to unequip the Item.
	const bool bIsUnequipped = EquipableManager->Unequip(ParentMenu->GetContextItem()->GetInventoryStackId());
	if (bIsUnequipped)
	{
		ParentMenu->DeactivateWidget();
		return; // Nothing else to do.
	}

	// TODO: EEquipmentSlot::None
	// Unequipping failed, check if this should be an equip.
	const bool bIsEquipped = EquipableManager->ASyncRequestEquip(ParentMenu->GetContextItem()->GetInventoryStackId(), EEquipmentSlot::None);
	if (bIsEquipped)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Construct inventory view failed: Could not equip nor unequip!"), *FString(__FUNCTION__));
		return;
	}

	ParentMenu->DeactivateWidget();
}
