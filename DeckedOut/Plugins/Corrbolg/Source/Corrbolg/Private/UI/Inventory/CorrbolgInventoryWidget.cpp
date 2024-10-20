// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryWidget.h"

#include "GameFramework/Pawn.h"

#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "Logging/CorrbolgLogChannels.h"
#include "UI/Prefabs/CorrbolgNavigationWidget.h"
#include "CommonVisibilitySwitcher.h"
#include "UI/Inventory/CorrbolgInventoryViewWidget.h"
#include "Inventory/CorrbolgInventorySettings.h"
#include "Utilities/CorrbolgUtilities.h"

void UCorrbolgInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructInventoryView();
	ConstructNavigationBar();
}

#pragma region Navigation bar
void UCorrbolgInventoryWidget::ConstructNavigationBar()
{
	if (!IsValid(NavigationBar))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to construct navigation bar: No NavigationBar widget!"), *FString(__FUNCTION__));
		return;
	}

	if (!IsValid(InventorySwitcher))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to construct navigation bar: No InventorySwitcher widget!"), *FString(__FUNCTION__));
		return;
	}

	NavigationBar->SetupSwitcher(InventorySwitcher);
}
#pragma endregion

#pragma region Inventory Overview
void UCorrbolgInventoryWidget::ConstructInventoryView()
{
	const UCorrbolgInventoryManagerComponent* const InventoryComponent = FCorrbolgUtilities::FindInventoryComponent(GetOwningPlayer());
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to construct inventory view: No inventory component!"), *FString(__FUNCTION__));
		return;
	}

	// Get all inventories, and create an inventory view widget for them.
	const TArray<TSoftObjectPtr<UCorrbolgInventorySettings>>& InventorySettings = InventoryComponent->GetInventorySettings();
	for (const TSoftObjectPtr<UCorrbolgInventorySettings>& SoftSettings : InventorySettings)
	{
		UCorrbolgInventorySettings* const Settings = SoftSettings.LoadSynchronous();
		if (!IsValid(Settings))
		{
			continue;
		}

		const FName WidgetName = MakeUniqueObjectName(this, UCorrbolgInventoryViewWidget::StaticClass());

		UCorrbolgInventoryViewWidget* const InventoryView = CreateWidget<UCorrbolgInventoryViewWidget>(this, InventoryViewClass.LoadSynchronous(), WidgetName);
		if (!IsValid(InventoryView))
		{
			continue;
		}

		InventorySwitcher->AddChild(InventoryView);

		// TODO: Setup the inventory view widget, like creating the slots for the tileview, the slots data class,
		// link the tile view changed events to update he EntryPreview to the highligted slot.
		InventoryView->Init(*Settings);
	}
}
#pragma endregion
