// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusCardReaderLayoutWidget.h"

#include "UI/Foundation/TartarusActionBarWidget.h"
#include "UI/PlayerMenu/Inventory/TartarusSubInventoryViewWidget.h"
#include "UI/Foundation/TartarusItemSelectionWidget.h"
#include "UI/Foundation/TartarusItemDetailsWidget.h"
#include "UI/Foundation/TartarusGameModifiersWidget.h"
#include "Player/TartarusPlayerController.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "CommonTileView.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"

UTartarusCardReaderLayoutWidget::UTartarusCardReaderLayoutWidget()
{
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = true;
	bAutoActivate = true;
}

void UTartarusCardReaderLayoutWidget::InitializeInventoryWidget()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the inventory widget: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the inventory widget: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	InventoryWidget->LinkInventory(InventoryComponent, SubInventoryId);
	InventoryWidget->SetLocalizedWidgetName(InventoryComponent->GetSubInventoryName(SubInventoryId));

	//InventoryWidget->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemClicked);
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusCardReaderLayoutWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(ActiveCardsWidget))
	{
		return nullptr;
	}
	
	return ActiveCardsWidget;
}

void UTartarusCardReaderLayoutWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// Populate the inventory Widget with the player inventory.
	InitializeInventoryWidget();

	// Link the Active Cards and the Inventory to display the selected item in the ItemDetailsWidget.
	InitializeItemInfoWidget();

	// Initialize the GameModifier Widget with the current Active Cards.
}
#pragma endregion

#pragma region ItemInfo
void UTartarusCardReaderLayoutWidget::InitializeItemInfoWidget()
{
	InventoryWidget->GetTileView()->OnItemSelectionChanged().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged);

	ActiveCardsWidget->GetTileView()->OnItemSelectionChanged().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged);
}

void UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged(UObject* Item)
{
	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		return;
	}

	ItemDetailsWidget->SetItemReference(WidgetData->GetItemId());
}
#pragma endregion
