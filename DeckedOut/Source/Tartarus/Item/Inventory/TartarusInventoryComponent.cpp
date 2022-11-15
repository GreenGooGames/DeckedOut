// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryComponent.h"

#include "Logging/TartarusLogChannels.h"

#pragma region FInventoryItemStack
FInventoryItemStack::FInventoryItemStack(const int32 NewItemId, const int32 NewStackSize)
{
	StackId = FGuid::NewGuid();

	ItemId = NewItemId;
	StackSize = NewStackSize;
}
#pragma endregion

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

FGuid UTartarusInventoryComponent::StoreItem(const int32 ItemId, const int32 StackSize)
{
	// Verify the given parameters.
	if (ItemId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: ItemId was invalid!"), __FUNCTION__);
		return FGuid();
	}

	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: StackSize was  <= 0 !"), __FUNCTION__);
		return FGuid();
	}

	// Search if there is a duplicate.
	int32 SlotIndex = FindItem(ItemId);

	if (SlotIndex != INDEX_NONE)
	{
		// Duplicate is found, increase the Stacksize.
		InventorySlots[SlotIndex].StackSize += StackSize;
	}
	else
	{
		// No duplicate is found, Is there a slot available to add a new entry?
		SlotIndex = FindItem(FTartarusHelpers::InvalidItemId);

		if (SlotIndex == INDEX_NONE)
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to store the item: Inventory is full!"), __FUNCTION__);
			return FGuid();
		}

		// Add a new entry.
		FInventoryItemStack NewStack = FInventoryItemStack(ItemId, StackSize);

		InventorySlots[SlotIndex] = NewStack;
	}

	OnInventoryChanged().Broadcast(EInventoryChanged::Stored, InventorySlots[SlotIndex].GetStackId(), InventorySlots[SlotIndex].StackSize);

	return InventorySlots[SlotIndex].GetStackId();
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

	OnInventoryChanged().Broadcast(EInventoryChanged::Retrieved, InventorySlots[StoredIndex].GetStackId(), InventorySlots[StoredIndex].StackSize);

	if (RemainingStackSize <= 0)
	{
		// The whole stack is retrieved, invalidate this slot.
		InventorySlots[StoredIndex] = FInventoryItemStack();
	}

	return true;
}

bool UTartarusInventoryComponent::FindStack(const FGuid& StackId, FInventoryItemStack& OutStack)
{
	for (const FInventoryItemStack& Stack : InventorySlots)
	{
		if (Stack.GetStackId() == StackId)
		{
			OutStack = Stack;
			return true;
		}
	}

	return false;
}

int32 UTartarusInventoryComponent::FindItem(const int32 ItemId) const
{
	for (int32 i = 0; i < InventorySlots.Num() - 1; i++)
	{
		const FInventoryItemStack& Stack = InventorySlots[i];

		if (Stack.GetItemId() == ItemId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}
