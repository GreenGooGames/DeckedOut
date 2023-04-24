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
#include "UI/Foundation/TartarusSwitcherWidget.h"
#include "UI/Inventory/TartarusInventoryInfoWidget.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"
#include "UI/Inventory/TartarusSubInventoryView.h"

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
		FString EnumToString = "";
		
		const UEnum* const InventoryTypePtr = FindObject<UEnum>(FTopLevelAssetPath(TEXT("/Script/Tartarus.EInventoryType")), true);
		if (IsValid(InventoryTypePtr))
		{
			EnumToString = InventoryTypePtr->GetNameStringByValue(static_cast<int64>(InventoryId));
		}

		const FString WidgetName = FString("Widget_SubInventory_").Append(EnumToString);

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
		SubInventoryView->SetLocalizedWidgetName(FText::FromString(EnumToString));

		// TODO: Does this also work for Gamepad?
		SubInventoryView->GetTileView()->OnItemIsHoveredChanged().AddUObject(this, &UTartarusInventoryWidget::HandleItemFocuschanged);
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
void UTartarusInventoryWidget::HandleItemFocuschanged(UObject* Item, bool bIsHovered)
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
