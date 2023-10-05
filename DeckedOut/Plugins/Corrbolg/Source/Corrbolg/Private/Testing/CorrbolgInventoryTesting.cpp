// Fill out your copyright notice in the Description page of Project Settings.

#include "Testing/CorrbolgInventoryTesting.h"

#include "Inventory/CorrbolgInventory.h"
#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "Logging/CorrbolgLogChannels.h"

bool UCorrbolgInventoryTesting::AddItemToInventory(UCorrbolgInventoryManagerComponent* const Inventory, const FGameplayTag& ItemFilter, const FGuid& ItemId)
{
	if (!IsValid(Inventory))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to add item to inventory, Invnetory is invalid."), *FString(__FUNCTION__));
		return false;
	}

	UCorrbolgInventoryEntryDefinition* const ItemDefinition = NewObject<UCorrbolgInventoryEntryDefinition>();
	if (!IsValid(ItemDefinition))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to add item to inventory, ItemDefinition is invalid."), *FString(__FUNCTION__));
		return false;
	}

	ItemDefinition->Init(ItemId, FText::FromString("DEBUG_Item"), FText::FromString("This is an item used for debugging.."), ItemFilter, TArray<UCorrbolgInventoryEntryFragment*>());
	
	const bool bIsItemStored = Inventory->StoreEntry(ItemDefinition);

	return bIsItemStored;
}

bool UCorrbolgInventoryTesting::RemoveItemFromInventory(UCorrbolgInventoryManagerComponent* const Inventory, const FGuid& ItemId)
{
	if (!IsValid(Inventory))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to remove item from inventory, Invnetory is invalid."), *FString(__FUNCTION__));
		return false;
	}

	const bool bIsItemRetrieved = Inventory->RetrieveEntry(ItemId) == nullptr ? false : true;

	return bIsItemRetrieved;
}
