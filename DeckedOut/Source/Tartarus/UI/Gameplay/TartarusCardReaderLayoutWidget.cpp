// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusCardReaderLayoutWidget.h"

#include "UI/Foundation/TartarusActionBarWidget.h"
#include "UI/Foundation/TartarusItemDetailsWidget.h"
#include "UI/Foundation/TartarusGameModifiersWidget.h"
#include "UI/Foundation/TartarusTransferWidget.h"
#include "Logging/TartarusLogChannels.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "Interaction/TartarusInteractableSourceComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"

UTartarusCardReaderLayoutWidget::UTartarusCardReaderLayoutWidget()
{
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = true;
	bAutoActivate = true;
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusCardReaderLayoutWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(TransferWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to get the focus target: TransferWidget is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}
	
	return TransferWidget;
}

void UTartarusCardReaderLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Initialize the inventory Widgets.
	InitializeTransferWidget();

	// Link the Active Cards and the Inventory to display the selected item in the ItemDetailsWidget.
	InitializeItemInfoWidget();
}
#pragma endregion

#pragma region ItemInfo
void UTartarusCardReaderLayoutWidget::InitializeItemInfoWidget()
{
	TransferWidget->OnEntrySelectionChanged().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged);
}

void UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged(UObject* Item)
{
	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to set the data for the ItemDetailsWidget: Selected Item widget data is invalid!"), *FString(__FUNCTION__));
		return;
	}

	ItemDetailsWidget->SetItemReference(WidgetData->GetItemId());
}
#pragma endregion

#pragma region Transfer
void UTartarusCardReaderLayoutWidget::InitializeTransferWidget()
{
	TransferWidget->LinkInventories(CardsSubInventoryId, FindCardReaderInventory(), FindPlayerInventory());
	TransferWidget->ActivateWidget();
}

UTartarusInventoryComponent* UTartarusCardReaderLayoutWidget::FindCardReaderInventory() const
{
	UTartarusInteractableSourceComponent* const PlayerInteractableComponent = GetOwningPlayer()->FindComponentByClass<UTartarusInteractableSourceComponent>();
	if (!IsValid(PlayerInteractableComponent))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the CardReader inventory: Unable to find the Interactable Component!"), *FString(__FUNCTION__));
		return nullptr;
	}
	
	AActor* const InteractingActor = Cast<AActor>(PlayerInteractableComponent->GetTarget().Get());
	if (!IsValid(InteractingActor))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the CardReader inventory: The Interacting Actor is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}
	
	UTartarusInventoryComponent* const CardReaderInventory = InteractingActor->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(CardReaderInventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the CardReader inventory: Unable to find the Inventory Component!"), *FString(__FUNCTION__));
		return nullptr;
	}

	return CardReaderInventory;
}

UTartarusInventoryComponent* UTartarusCardReaderLayoutWidget::FindPlayerInventory() const
{
	UTartarusInventoryComponent* const InventoryComponent = GetOwningPlayer()->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the Player inventory: Unable to find the Inventory Component!"), *FString(__FUNCTION__));
		return nullptr;
	}

	return InventoryComponent;
}
#pragma endregion
