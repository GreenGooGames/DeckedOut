// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryData.h"

#include "System/TartarusHelpers.h"
#include "Logging/TartarusLogChannels.h"

#pragma region FInventoryItemStack
FInventoryStack::FInventoryStack(const EInventoryType InventoryId, const FPrimaryAssetId NewEntryId, const int32 NewStackSize)
{
	StackId = FInventoryStackId(InventoryId);

	EntryId = NewEntryId;
	StackSize = NewStackSize;
}
#pragma endregion

#pragma region FSubInventory
FInventoryStackId FSubInventory::AddEntry(const FPrimaryAssetId EntryId, const bool bTryStack, const int32 StackSize)
{
	// Search if there is a stackable duplicate.
	const FInventoryStackId* StackId = FindStackId(EntryId);

	if (bTryStack && StackId)
	{
		// Stackable Duplicate is found, increase the Stacksize.
		FInventoryStack* const Stack = FindEditableStack(*StackId);
		Stack->StackSize += StackSize;
	}
	else
	{
		// No duplicate is found/item is non-stackable, Is there a slot available to add a new entry?
		StackId = FindStackId(FTartarusHelpers::InvalidItemId);

		if (!StackId)
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to store the item: Inventory is full!"), *FString(__FUNCTION__));
			return FInventoryStackId();
		}

		// Add a new entry.
		const FInventoryStack NewStack = FInventoryStack(InventoryId, EntryId, StackSize);

		FInventoryStack* const Stack = FindEditableStack(*StackId);
		*Stack = NewStack;
	}

	return *StackId;
}

bool FSubInventory::RemoveEntry(const FInventoryStackId& StackId, const int32 StackSize)
{
	// Verify that an valid attempt is being made to reduce the stack and not increase it.
	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack: StackSize to reduce with was  <= 0 !"), *FString(__FUNCTION__));
		return false;
	}

	FInventoryStack* const EntryStack = FindEditableStack(StackId);
	if (!EntryStack)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack: The given StackId was not found!"), *FString(__FUNCTION__));
		return false;
	}

	// Check if there are enough stored items.
	if (EntryStack->StackSize < StackSize)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack:: trying to retrieve more items than there are stored!"), *FString(__FUNCTION__));
		return false;
	}

	// Reduce the stored stacksize with the reduction.
	EntryStack->StackSize -= StackSize;

	if (EntryStack->StackSize <= 0)
	{
		// The whole stack is retrieved, invalidate this slot.
		*EntryStack = FInventoryStack();
	}

	return true;
}

const FInventoryStackId* const FSubInventory::FindStackId(const FPrimaryAssetId EntryId) const
{
	for (const FInventoryStack& Entry : Content)
	{
		if (Entry.GetEntryId() == EntryId)
		{
			return &Entry.GetStackId();
		}
	}

	return nullptr;
}

const FInventoryStack* FSubInventory::FindStack(const FInventoryStackId& StackId) const
{
	for (const FInventoryStack& Stack : Content)
	{
		if (Stack.GetStackId() == StackId)
		{
			return &Stack;
		}
	}

	return nullptr;
}

FInventoryStack* FSubInventory::FindEditableStack(const FInventoryStackId& StackId)
{
	for (FInventoryStack& Stack : Content)
	{
		if (Stack.GetStackId() == StackId)
		{
			return &Stack;
		}
	}

	return nullptr;
}
#pragma endregion
