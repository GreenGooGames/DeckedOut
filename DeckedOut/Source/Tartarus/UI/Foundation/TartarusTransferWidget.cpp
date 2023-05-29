// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusTransferWidget.h"

#include "UI/PlayerMenu/Inventory/TartarusSubInventoryViewWidget.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "CommonTileView.h"

void UTartarusTransferWidget::LinkInventories(const FGameplayTag& SubInventoryId, UTartarusInventoryComponent* const HostInventory, UTartarusInventoryComponent* const ClientInventory)
{
	// Verify the Input Parameters.
	if (!SubInventoryId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to Initialize the TransferWidget: The given SubInventory is invalid!"), *FString(__FUNCTION__));
		return;
	}

	if (!IsValid(HostInventory) || !IsValid(ClientInventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to Initialize the TransferWidget: Host and/or Client Inventory is invalid!"), *FString(__FUNCTION__));
		return;
	}

	// Verify this widget its data.
	if (!IsValid(HostInventoryWidget) || !IsValid(ClientInventoryWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to Initialize the TransferWidget: Host and/or Client Inventory Widget is invalid!"), *FString(__FUNCTION__));
		return;
	}

	// Initialize the SubInventory Widgets.
	HostInventoryWidget->LinkInventory(HostInventory, SubInventoryId);
	HostInventoryWidget->SetLocalizedWidgetName(HostInventory->GetSubInventoryName(SubInventoryId));
	HostInventoryWidget->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusTransferWidget::HandleHostInventoryItemClicked);
	HostInventoryWidget->GetTileView()->OnItemSelectionChanged().AddLambda([&](UObject* SelectedItem) {OnEntrySelectionChangedHandle.Broadcast(SelectedItem); });
	HostInventoryWidget->ActivateWidget();

	ClientInventoryWidget->LinkInventory(ClientInventory, SubInventoryId);
	ClientInventoryWidget->SetLocalizedWidgetName(ClientInventory->GetSubInventoryName(SubInventoryId));
	ClientInventoryWidget->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusTransferWidget::HandleClientInventoryItemClicked);
	ClientInventoryWidget->GetTileView()->OnItemSelectionChanged().AddLambda([&](UObject* SelectedItem) {OnEntrySelectionChangedHandle.Broadcast(SelectedItem); });
	ClientInventoryWidget->ActivateWidget();
}

bool UTartarusTransferWidget::TransferToInventory(const UTartarusInventorySlotWidgetData* const InventorySlotWidgetData, UTartarusInventoryComponent* const SourceInventory, UTartarusInventoryComponent* const TargetInventory)
{
	// Verify the input parameters.
	if (!IsValid(InventorySlotWidgetData))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to transfer item: SlotWidgetData is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	if (!IsValid(SourceInventory) || !IsValid(TargetInventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to transfer item: Either Source or Target inventory is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the required data to create a copy of the Item in the Other inventory.
	const FPrimaryAssetId& ItemAssestId = InventorySlotWidgetData->GetItemId();
	const FGameplayTag& ItemSubInventoryId = InventorySlotWidgetData->GetInventoryStackId().GetInventoryId();
	
	// Store the Item in the Target inventory.
	// TODO: Find out if the item is stackable or not instead of passing false, except for when storing in the CardReader Inventory then we don't stack.
	// TODO: Magic Number 1.
	const FInventoryStackId& StoredStackId = TargetInventory->StoreEntry(ItemSubInventoryId, ItemAssestId, false, 1);
	if (!StoredStackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to transfer item: Could not store the entry in the target inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Retrieve the Item from the Host as it has been transfered to the Target.
	// TODO: Magic Number 1.
	const bool bHasRetrieved = SourceInventory->RetrieveEntry(InventorySlotWidgetData->GetInventoryStackId(), 1);
	if (!bHasRetrieved)
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Item duplication, Entry was stored in the target inventory but could not be retrieved from the source inventory!"), *FString(__FUNCTION__));
		return false;
	}

	return true;
}

void UTartarusTransferWidget::HandleHostInventoryItemClicked(UObject* Item)
{
	UTartarusInventoryComponent* const SourceInventory = HostInventoryWidget->GetInventory();
	UTartarusInventoryComponent* const TargetInventory = ClientInventoryWidget->GetInventory();
	UTartarusInventorySlotWidgetData* const SlotWidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);

	TransferToInventory(SlotWidgetData, SourceInventory, TargetInventory);
}

void UTartarusTransferWidget::HandleClientInventoryItemClicked(UObject* Item)
{
	UTartarusInventoryComponent* const SourceInventory = ClientInventoryWidget->GetInventory();
	UTartarusInventoryComponent* const TargetInventory = HostInventoryWidget->GetInventory();
	UTartarusInventorySlotWidgetData* const SlotWidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);

	TransferToInventory(SlotWidgetData, SourceInventory, TargetInventory);
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusTransferWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(HostInventoryWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to get the focus target: HostInventoryWidget is invalid!"), *FString(__FUNCTION__));
		return nullptr;
	}

	return HostInventoryWidget;
}
#pragma endregion
