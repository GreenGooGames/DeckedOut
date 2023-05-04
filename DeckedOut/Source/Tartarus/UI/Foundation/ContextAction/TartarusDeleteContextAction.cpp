// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusDeleteContextAction.h"

#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusDeleteContextAction::ExecuteAction()
{
	const ATartarusPlayerController* const PlayerController = ParentMenu->GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to perform Delete action: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to perform Delete action: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	if (!IsValid(ParentMenu->GetContextItem().Get()))
	{
		return;
	}

	const FInventoryStackId& InventoryStackId = ParentMenu->GetContextItem()->GetInventoryStackId();
	bool bIsRetrieved = InventoryComponent->RetrieveEntry(InventoryStackId, 1);

	if (!bIsRetrieved)
	{
		return;
	}

	// If the ID has turned invalid, then no more items are left in the inventory. Close the context menu.
	if (!InventoryStackId.IsValid())
	{
		ParentMenu->DeactivateWidget();
	}
}
