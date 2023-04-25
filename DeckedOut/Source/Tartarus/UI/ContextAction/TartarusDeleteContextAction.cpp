// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContextAction/TartarusDeleteContextAction.h"

#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusContextMenu.h"
#include "UI/Foundation/TartarusContextMenu.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusDeleteContextAction::ExecuteAction()
{
	const ATartarusPlayerController* const PlayerController = ParentMenu->GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No inventory found!"), *FString(__FUNCTION__));
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

	const FInventoryStack* const InventoryStack = InventoryComponent->GetOverviewSingle(InventoryStackId);
	if (InventoryStack == nullptr)
	{
		ParentMenu->DeactivateWidget();
	}
}
