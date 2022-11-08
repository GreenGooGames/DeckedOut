// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryComponent.h"

#include "Logging/TartarusLogChannels.h"

// Sets default values for this component's properties
UTartarusInventoryComponent::UTartarusInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UTartarusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize all slots.
	InventorySlots.SetNum(NumberOfSlots);
}

bool UTartarusInventoryComponent::StoreItem(const int32 ItemId, const int32 StackSize)
{
	// Verify the given parameters.
	if (ItemId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: ItemId was invalid!"), __FUNCTION__);
		return false;
	}

	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: StackSize was  <= 0 !"), __FUNCTION__);
		return false;
	}

	// Search if there is a duplicate.
	const int32 DuplicateIndex = FindItem(ItemId);

	if (DuplicateIndex != INDEX_NONE)
	{
		// Duplicate is found, increase the Stacksize.
		InventorySlots[DuplicateIndex].StackSize += StackSize;
	}
	else
	{
		// No duplicate is found, Is there a slot available to add a new entry?
		const int32 AvailableSlotIndex = FindItem(FTartarusHelpers::InvalidItemId);

		if (AvailableSlotIndex == INDEX_NONE)
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to store the item: Inventory is full!"), __FUNCTION__);
			return false;
		}

		// Add a new entry.
		FInventoryItemStack NewStack = FInventoryItemStack();
		NewStack.ItemId = ItemId;
		NewStack.StackSize = StackSize;

		InventorySlots[AvailableSlotIndex] = NewStack;
	}

	return true;
}

bool UTartarusInventoryComponent::RetrieveItem(const int32 ItemId, const int32 StackSize)
{
	// Verify the given parameters.
	if (ItemId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: ItemId was invalid!"), __FUNCTION__);
		return false;
	}

	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: StackSize was  <= 0 !"), __FUNCTION__);
		return false;
	}

	// Find the Item.
	const int32 StoredIndex = FindItem(ItemId);

	if (StoredIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: the item was not stored in the inventory!"), __FUNCTION__);
		return false;
	}

	// Check if there are enough stored items.
	const int32 StoredStackSize = InventorySlots[StoredIndex].StackSize;

	if (StoredStackSize < StackSize)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: trying to retrieve more items than there are stored!"), __FUNCTION__);
		return false;
	}

	// Reduce the stored stacksize with the retrieved amount.
	const int32 RemainingStackSize = InventorySlots[StoredIndex].StackSize -= StackSize;

	if (RemainingStackSize <= 0)
	{
		// The whole stack is retrieved, invalidate this slot.
		InventorySlots[StoredIndex] = FInventoryItemStack();
	}

	return true;
}

int32 UTartarusInventoryComponent::FindItem(const int32 ItemId) const
{
	for (int32 i = 0; i < InventorySlots.Num() - 1; i++)
	{
		const FInventoryItemStack& Stack = InventorySlots[i];

		if (Stack.ItemId == ItemId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}
