// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryWidget.h"

#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "Logging/CorrbolgLogChannels.h"
#include "UI/Prefabs/CorrbolgNavigationWidget.h"
#include "CommonVisibilitySwitcher.h"

UCorrbolgInventoryManagerComponent* UCorrbolgInventoryWidget::FindInventoryComponent() const
{
	const APlayerController* const PlayerController = GetOwningPlayer<APlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to find inventory component: No player controller!"), *FString(__FUNCTION__));
		return nullptr;
	}

	UCorrbolgInventoryManagerComponent* const InventoryComponent = PlayerController->FindComponentByClass<UCorrbolgInventoryManagerComponent>();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to find inventory component: No inventory found!"), *FString(__FUNCTION__));
		return nullptr;
	}

	return InventoryComponent;
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
	const UCorrbolgInventoryManagerComponent* const InventoryComponent = FindInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to construct inventory view: No inventory component!"), *FString(__FUNCTION__));
		return;
	}

	// Get all inventories
	// For each inventory
	// Get the UI name (stored in the inventory settings)
	// Then create an inventory overview.
}
#pragma endregion
