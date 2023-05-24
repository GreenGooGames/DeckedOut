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
#include "Interaction/TartarusInteractableSourceComponent.h"

UTartarusCardReaderLayoutWidget::UTartarusCardReaderLayoutWidget()
{
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = true;
	bAutoActivate = true;
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusCardReaderLayoutWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(CardReaderInventoryWidget))
	{
		return nullptr;
	}
	
	return CardReaderInventoryWidget;
}

void UTartarusCardReaderLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Populate the inventory Widgets.
	InitializeCardReaderInventoryWidget();
	InitializePlayerInventoryWidget();

	// Link the Active Cards and the Inventory to display the selected item in the ItemDetailsWidget.
	InitializeItemInfoWidget();
}
#pragma endregion

#pragma region ItemInfo
void UTartarusCardReaderLayoutWidget::InitializeItemInfoWidget()
{
	PlayerInventoryWidget->GetTileView()->OnItemSelectionChanged().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged);

	CardReaderInventoryWidget->GetTileView()->OnItemSelectionChanged().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleItemSelectionChanged);
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

#pragma region Inventory
bool UTartarusCardReaderLayoutWidget::TransferToInventory(const UTartarusInventorySlotWidgetData* const ItemData, UTartarusInventoryComponent* const Host, UTartarusInventoryComponent* const Recipient)
{
	FPrimaryAssetId ItemAssestId = ItemData->GetItemId();

	bool bHasRetrieved = Host->RetrieveEntry(ItemData->GetInventoryStackId(), 1);
	if (!bHasRetrieved || !ItemAssestId.IsValid())
	{
		return false;
	}

	// TODO: Find out if the item is stackable or not instead of passing false.
	FInventoryStackId StoredStackId = Recipient->StoreEntry(SubInventoryId, ItemAssestId, false, 1);
	if (!StoredStackId.IsValid())
	{
		Host->StoreEntry(SubInventoryId, ItemData->GetItemId(), false, 1);
		return false;
	}

	return true;
}

#pragma region CardReaderInventory
void UTartarusCardReaderLayoutWidget::InitializeCardReaderInventoryWidget()
{
	UTartarusInteractableSourceComponent* const PlayerInteractableComponent = GetOwningPlayer()->FindComponentByClass<UTartarusInteractableSourceComponent>();
	if (!IsValid(PlayerInteractableComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the CardReader inventory widget: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	AActor* const InteractingActor = Cast<AActor>(PlayerInteractableComponent->GetTarget().Get());
	if (!IsValid(InteractingActor))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the CardReader inventory widget: Not interacting!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const CardReaderInventory = InteractingActor->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(CardReaderInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the CardReader inventory widget: No inventory on the interacting actor!"), *FString(__FUNCTION__));
		return;
	}

	CardReaderInventoryWidget->LinkInventory(CardReaderInventory, SubInventoryId);
	CardReaderInventoryWidget->SetLocalizedWidgetName(CardReaderInventory->GetSubInventoryName(SubInventoryId));

	CardReaderInventoryWidget->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandleCardReaderInventoryItemClicked);
}

void UTartarusCardReaderLayoutWidget::HandleCardReaderInventoryItemClicked(UObject* Item)
{
	// Transfer the Item to the Player Inventory.
	UTartarusInventoryComponent* const PlayerInventory = PlayerInventoryWidget->GetInventory();
	UTartarusInventoryComponent* const CardReaderInventory = CardReaderInventoryWidget->GetInventory();
	if (!IsValid(PlayerInventory) || !IsValid(CardReaderInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle CardReader invetory item click: Either player or card reader inventory is invalid!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle CardReader invetory item click: Clicked Item is not of SlotWidgetData!"), *FString(__FUNCTION__));
		return;
	}

	const bool bHasTransfered = TransferToInventory(WidgetData, CardReaderInventory, PlayerInventory);
	if (!bHasTransfered)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle CardReader inventory item click: Failed to transfer the item!"), *FString(__FUNCTION__));
		return;
	}

	// Remove the modifier from the GameInstance.
}
#pragma endregion

#pragma region PlayerInventory
void UTartarusCardReaderLayoutWidget::InitializePlayerInventoryWidget()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the Player inventory widget: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to initialize the Player inventory widget: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	PlayerInventoryWidget->LinkInventory(InventoryComponent, SubInventoryId);
	PlayerInventoryWidget->SetLocalizedWidgetName(InventoryComponent->GetSubInventoryName(SubInventoryId));

	PlayerInventoryWidget->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusCardReaderLayoutWidget::HandlePlayerInventoryItemClicked);
}

void UTartarusCardReaderLayoutWidget::HandlePlayerInventoryItemClicked(UObject* Item)
{
	// Transfer the Item to the Card Reader Inventory.
	UTartarusInventoryComponent* const PlayerInventory = PlayerInventoryWidget->GetInventory();
	UTartarusInventoryComponent* const CardReaderInventory = CardReaderInventoryWidget->GetInventory();
	if (!IsValid(PlayerInventory) || !IsValid(CardReaderInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle Player inventory item click: Either player or card reader inventory is invalid!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle PLayer inventory item click: Clicked Item is not of SlotWidgetData!"), *FString(__FUNCTION__));
		return;
	}

	const bool bHasTransfered = TransferToInventory(WidgetData, PlayerInventory, CardReaderInventory);
	if (!bHasTransfered)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to handle CardReader inventory item click: Failed to transfer the item!"), *FString(__FUNCTION__));
		return;
	}

	// Add the modifer to the GameInstance.
}
#pragma endregion
#pragma endregion