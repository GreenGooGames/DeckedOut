// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventoryWidget.h"

#include "CommonTileView.h"
#include "CommonVisibilitySwitcher.h"
#include "Engine/Engine.h"
#include "Input/CommonUIInputTypes.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/ContextAction/TartarusContextAction.h"
#include "UI/Foundation/TartarusContextMenu.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"
#include "UI/Inventory/TartarusInventoryInfoWidget.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"
#include "UI/Inventory/TartarusSubInventoryView.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UTartarusInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
	ConstructInventoryView();
	SetupMenuSwitcher();

	SelectedItemInfo->DeactivateWidget();
}

#pragma region Sub-Inventory
void UTartarusInventoryWidget::ConstructInventoryView()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	// Create a template sub inventory for each subinventory owned by the inventory component.
	const TArray<EInventoryType> SubInventoryIds = InventoryComponent->GetSubInventoryIds();

	for (const EInventoryType& InventoryId : SubInventoryIds)
	{
		const FString WidgetName = FString("Widget_SubInventory_").Append(InventoryComponent->GetSubInventoryName(InventoryId).ToString());

		// TODO: Async load the SubInventoryClass.
		// Instantiate a template subinventory.
		UTartarusSubInventoryView* const SubInventoryView = CreateWidget<UTartarusSubInventoryView>(GetWorld(), SubInventoryViewClass.LoadSynchronous(), *WidgetName);
		if (!IsValid(SubInventoryView))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: Could not instaniate a subinventory view!"), *FString(__FUNCTION__));
			return;
		}

		// Set the new widget as a child of the VisibilitySwitcher.
		const UPanelSlot* const PanelSlot = SubInventoryVisibilitySwitcher->AddChild(Cast<UWidget>(SubInventoryView));
		if (!IsValid(PanelSlot))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: Failed to add subinventoryview as child of the visibility switcher.!"), *FString(__FUNCTION__));
			return;
		}

		SubInventoryView->LinkInventory(InventoryId);
		SubInventoryView->SetLocalizedWidgetName(InventoryComponent->GetSubInventoryName(InventoryId));

		// TODO: Does this also work for Gamepad?
		SubInventoryView->GetTileView()->OnItemIsHoveredChanged().AddUObject(this, &UTartarusInventoryWidget::HandleItemFocusChanged);
		SubInventoryView->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusInventoryWidget::HandleItemClicked);
	}
}
#pragma endregion

#pragma region BoundActions
void UTartarusInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SelectBoundActionArguments = FBindUIActionArgs(SelectInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectAction));
	SelectBoundActionArguments.bDisplayInActionBar = true;

	SelectActionHandle = RegisterUIActionBinding(SelectBoundActionArguments);

	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UTartarusInventoryWidget::HandleSelectAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(*FString(__FUNCTION__)));
#endif
}

void UTartarusInventoryWidget::HandleSortAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(*FString(__FUNCTION__)));
#endif
}
#pragma endregion

#pragma region Navbar
void UTartarusInventoryWidget::SetupMenuSwitcher()
{
	if (SubInventoryMenuSwitcher && SubInventoryVisibilitySwitcher)
	{
		SubInventoryMenuSwitcher->LinkVisibilitySwitcher(SubInventoryVisibilitySwitcher);
	}
}
#pragma endregion

#pragma region ItemInfo
void UTartarusInventoryWidget::HandleItemFocusChanged(UObject* Item, bool bIsHovered)
{
	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		return;
	}

	if (!bIsHovered)
	{
		return;
	}

	SelectedItemInfo->SetItemReference(WidgetData->GetItemId());
}
#pragma endregion

#pragma region Context
void UTartarusInventoryWidget::HandleItemClicked(UObject* Item)
{
	// Get the data of the item that is being represented by the clicked slot.
	UTartarusInventorySlotWidgetData* SlotData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(SlotData))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create show Context Menu, Slot is not of class UTartarusInventorySlotData!"), *FString(__FUNCTION__));
		return;
	}

	if (SlotData->GetItemId() == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create show Context Menu, Slot does not contain a valid item!"), *FString(__FUNCTION__));
		return;
	}

	// Create a new Widget or re-use one in the pool to represent the context menu.
	if (!IsValid(ContextStack))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create show Context Menu, ContextStack is invalid!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusContextMenu* const ContextMenu = ContextStack->AddWidget<UTartarusContextMenu>(ContextMenuTemplate.LoadSynchronous());
	if (!IsValid(ContextMenu))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create show Context Menu, Could not create a contextmenu widget!"), *FString(__FUNCTION__));
		return;
	}

	// A Context Menu is now available, store a reference to the item that it has to represent.
	ContextMenu->SetContextItem(SlotData);

	// Create all the buttons required to show.
	if (ContextMenu->ContainsEntries())
	{
		return;
	}

	UTartarusSubInventoryView* const SubInventoryWidget = Cast<UTartarusSubInventoryView>(SubInventoryVisibilitySwitcher->GetActiveWidget());
	if (!IsValid(SubInventoryWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create show Context Menu, SubInventoryWidget is invalid!"), *FString(__FUNCTION__));
		return;
	}

	const TArray<UTartarusContextAction*>& ContextActions = SubInventoryWidget->GetContextActions();
	for (UTartarusContextAction* const ContextAction : ContextActions)
	{
		ContextAction->SetParentMenu(ContextMenu);
		ContextMenu->AddEntry(ContextAction);
	}
}
#pragma endregion
