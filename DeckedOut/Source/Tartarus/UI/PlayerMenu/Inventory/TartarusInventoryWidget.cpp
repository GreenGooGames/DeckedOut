// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerMenu/Inventory/TartarusInventoryWidget.h"

#include "CommonTileView.h"
#include "CommonVisibilitySwitcher.h"
#include "Engine/Engine.h"
#include "Input/CommonUIInputTypes.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerCharacter.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"
#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"
#include "UI/PlayerMenu/Inventory/TartarusInventoryInfoWidget.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "UI/PlayerMenu/Inventory/TartarusSubInventoryViewWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/PlayerMenu/TartarusPlayerMenuLayoutWidget.h"
#include "UI/Foundation/ContextAction/TartarusContextActionBulk.h"

void UTartarusInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
	ConstructInventoryView();
	SetupMenuSwitcher();

	SelectedItemInfo->DeactivateWidget();

	SubInventoryMenuSwitcher->BindVisibilityToActivation(ContextMenu);
	SubInventoryMenuSwitcher->SetBindVisibilities(ESlateVisibility::HitTestInvisible, ESlateVisibility::Visible, false);

	ContextMenu->OnActivated().AddLambda([&]() {SubInventoryVisibilitySwitcher->SetVisibility(ESlateVisibility::HitTestInvisible); });
	ContextMenu->OnDeactivated().AddLambda([&]() {SubInventoryVisibilitySwitcher->SetVisibility(ESlateVisibility::Visible); });

	ContextMenu->OnActivated().AddLambda([&]() 
		{
			UTartarusPlayerMenuLayoutWidget* PlayerLayoutWidget = FindParentOfType<UTartarusPlayerMenuLayoutWidget>();
			if (IsValid(PlayerLayoutWidget))
			{
				PlayerLayoutWidget->GetMenuSwitcher()->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		});
	ContextMenu->OnDeactivated().AddLambda([&]() 
		{
			UTartarusPlayerMenuLayoutWidget* PlayerLayoutWidget = FindParentOfType<UTartarusPlayerMenuLayoutWidget>();
			if (IsValid(PlayerLayoutWidget))
			{
				PlayerLayoutWidget->GetMenuSwitcher()->SetVisibility(ESlateVisibility::Visible);
			}
		});

	ContextMenu->DeactivateWidget();
}

UWidget* UTartarusInventoryWidget::NativeGetDesiredFocusTarget() const
{
	if (IsValid(SubInventoryVisibilitySwitcher))
	{
		return SubInventoryVisibilitySwitcher->GetActiveWidget();
	}
	return nullptr;
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

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	// Create a template sub inventory for each subinventory owned by the inventory component.
	const TArray<FGameplayTag> SubInventoryIds = InventoryComponent->GetSubInventoryIds();

	for (const FGameplayTag& InventoryId : SubInventoryIds)
	{
		const FString WidgetName = FString("Widget_SubInventory_").Append(InventoryComponent->GetSubInventoryName(InventoryId).ToString());

		// TODO: Async load the SubInventoryClass.
		// Instantiate a template subinventory.
		UTartarusSubInventoryViewWidget* const SubInventoryView = CreateWidget<UTartarusSubInventoryViewWidget>(GetWorld(), SubInventoryViewClass.LoadSynchronous(), *WidgetName);
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

		SubInventoryView->LinkInventory(InventoryComponent, InventoryId);
		SubInventoryView->SetLocalizedWidgetName(InventoryComponent->GetSubInventoryName(InventoryId));

		SubInventoryView->GetTileView()->OnItemSelectionChanged().AddUObject(this, &UTartarusInventoryWidget::HandleItemSelectionChanged);
		SubInventoryView->GetTileView()->OnItemClicked().AddUObject(this, &UTartarusInventoryWidget::HandleItemClicked);
	}
}
#pragma endregion

#pragma region BoundActions
void UTartarusInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInventoryEnhancedInputAction, true, FSimpleDelegate::CreateUObject(this, &UTartarusInventoryWidget::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UTartarusInventoryWidget::HandleSortAction()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to sort the inventory: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to sort the inventory: No inventory found!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusSubInventoryViewWidget* const SubInventoryWidget = Cast<UTartarusSubInventoryViewWidget>(SubInventoryVisibilitySwitcher->GetActiveWidget());
	if (!IsValid(SubInventoryWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to sort the inventory: SubInventoryWidget is invalid!"), *FString(__FUNCTION__));
		return;
	}

	InventoryComponent->SortInventory(SubInventoryWidget->GetInventoryId());
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
void UTartarusInventoryWidget::HandleItemSelectionChanged(UObject* Item)
{
	UTartarusInventorySlotWidgetData* const WidgetData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(WidgetData))
	{
		return;
	}

	SelectedItemInfo->SetItemReference(WidgetData->GetItemId());
}
#pragma endregion

#pragma region Context
void UTartarusInventoryWidget::HandleItemClicked(UObject* Item)
{
	// Validate that we can create a Modal Context Widget.
	if (!IsValid(ContextMenu))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show Context Menu: ContextMenu is invalid!"), *FString(__FUNCTION__));
		return;
	}

	const UTartarusInventorySlotWidgetData* const SlotData = Cast<UTartarusInventorySlotWidgetData>(Item);
	if (!IsValid(SlotData))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show Context Menu: The Item clicked is not of an allowed class!"), *FString(__FUNCTION__));
		return;
	}

	if (SlotData->GetItemId() == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show Context Menu: The Item Id is invalid!"), *FString(__FUNCTION__));
		return;
	}

	ensureMsgf(ContextMenu->IsModal(), TEXT("%s: Showing a Context Widget that is not Modal. This widget should be modal to prevent action/input touring to other widgets."), *FString(__FUNCTION__));

	// Show the Context Menu if needed and set the correct data.
	if (!ContextMenu->IsActivated())
	{
		ContextMenu->ActivateWidget();
	}

	// Getting the slot data inventory id is good enough here instead of getting the current active subinventory widget.
	ContextMenu->SetContext(SlotData, AvailableContextActions[SlotData->GetInventoryStackId().GetInventoryId()]->GetContextActions());
}

template<typename T>
T* UTartarusInventoryWidget::FindParentOfType()
{
	// https://dev.epicgames.com/community/snippets/BXj/unreal-engine-get-parent-widget-of-class
	if (UObject* Top = GetParent())
	{
		for (;;)
		{
			UObject* CurrentOuter = Top->GetOuter();
			if (CurrentOuter->IsA(T::StaticClass()))
			{
				return Cast<T>(CurrentOuter);
			}
			else if (CurrentOuter->IsA<UWidgetTree>() || CurrentOuter->IsA<UWidget>())
			{
				Top = CurrentOuter;
			}
			else
			{
				return nullptr;
			}
		}
	}

	return nullptr;
}
#pragma endregion
