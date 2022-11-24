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

#pragma optimize("", off)
FGuid UTartarusInventoryComponent::StoreItem(const int32 ItemId, const int32 StackSize)
{
	// Verify the given parameters.
	if (ItemId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: ItemId was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: StackSize was  <= 0 !"), *FString(__FUNCTION__));
		return FGuid();
	}

	const bool bIsStackableItem = ItemId > FTartarusHelpers::InvalidItemId;
	if (!bIsStackableItem && StackSize > 1)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the item: Trying to add multiples of a unique item in the same stack!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Search if there is a stackable duplicate.
	int32 SlotIndex = FindSlot(ItemId);

	if (bIsStackableItem && SlotIndex != INDEX_NONE)
	{
		// Stackable Duplicate is found, increase the Stacksize.
		InventorySlots[SlotIndex].StackSize += StackSize;
	}
	else
	{
		// No duplicate is found/item is non-stackable, Is there a slot available to add a new entry?
		SlotIndex = FindSlot(FTartarusHelpers::InvalidItemId);

		if (SlotIndex == INDEX_NONE)
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to store the item: Inventory is full!"), *FString(__FUNCTION__));
			return FGuid();
		}

		// Add a new entry.
		FInventoryItemStack NewStack = FInventoryItemStack(ItemId, StackSize);

		InventorySlots[SlotIndex] = NewStack;
	}

	OnInventoryChanged().Broadcast(EInventoryChanged::Stored, InventorySlots[SlotIndex].GetStackId(), InventorySlots[SlotIndex].StackSize);

	return InventorySlots[SlotIndex].GetStackId();
}
#pragma optimize("", on)

bool UTartarusInventoryComponent::RetrieveItem(const int32 ItemId, const int32 StackSize)
{
	// Verify the given ItemId.
	if (ItemId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: ItemId was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const int32 SlotIndex = FindSlot(ItemId);
	const bool bHasReduced = ReduceStack(SlotIndex, StackSize);

	return bHasReduced;
}

bool UTartarusInventoryComponent::RetrieveItem(const FGuid& StackId, const int32 StackSize)
{
	// Verify the given StackId.
	if (!StackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the item: ItemId was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const int32 SlotIndex = FindSlot(StackId);
	const bool bHasReduced = ReduceStack(SlotIndex, StackSize);

	return bHasReduced;
}

const TArray<const FInventoryItemStack*> UTartarusInventoryComponent::GetOverviewMulti(const int32 ItemId) const
{
	TArray<const FInventoryItemStack*> MatchingItemSlots = TArray<const FInventoryItemStack*>();
	
	for (const FInventoryItemStack& Stack : InventorySlots)
	{
		if (Stack.GetItemId() == ItemId)
		{
			MatchingItemSlots.Add(&Stack);
		}
	}

	return MatchingItemSlots;
}

const FInventoryItemStack* UTartarusInventoryComponent::GetOverviewSingle(const FGuid StackId) const
{
	int32 SlotIndex = FindSlot(StackId);

	if (SlotIndex == INDEX_NONE)
	{
		return nullptr;
	}

	return &InventorySlots[SlotIndex];
}

int32 UTartarusInventoryComponent::GetAvailableSlotCount() const
{
	int32 NumAvailableSlots = 0;

	for (const FInventoryItemStack& Stack : InventorySlots)
	{
		if (Stack.GetItemId() == FTartarusHelpers::InvalidItemId)
		{
			NumAvailableSlots++;
		}
	}

	return NumAvailableSlots;
}

bool UTartarusInventoryComponent::Contains(const int32 ItemId) const
{
	for (const FInventoryItemStack& Stack : InventorySlots)
	{
		if (Stack.GetItemId() == ItemId)
		{
			return true;
		}
	}

	return false;
}

bool UTartarusInventoryComponent::Contains(const FGuid StackId) const
{
	for (const FInventoryItemStack& Stack : InventorySlots)
	{
		if (Stack.GetStackId() == StackId)
		{
			return true;
		}
	}

	return false;
}

int32 UTartarusInventoryComponent::FindSlot(const int32 ItemId) const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		const FInventoryItemStack& Stack = InventorySlots[i];

		if (Stack.GetItemId() == ItemId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int32 UTartarusInventoryComponent::FindSlot(const FGuid& StackId) const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		const FInventoryItemStack& Stack = InventorySlots[i];

		if (Stack.GetStackId() == StackId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool UTartarusInventoryComponent::ReduceStack(const int32 SlotIndex, const int32 StackSize)
{
	// Verify that a correct slot is given.
	if (SlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack: the item was not stored in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Verify that an valid attempt is being made to reduce the stack and not increase it.
	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack: StackSize to reduce with was  <= 0 !"), *FString(__FUNCTION__));
		return false;
	}

	// Check if there are enough stored items.
	const int32 StoredStackSize = InventorySlots[SlotIndex].StackSize;

	if (StoredStackSize < StackSize)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to reduce stack:: trying to retrieve more items than there are stored!"), *FString(__FUNCTION__));
		return false;
	}

	// Reduce the stored stacksize with the reduction.
	const int32 RemainingStackSize = InventorySlots[SlotIndex].StackSize -= StackSize;

	OnInventoryChanged().Broadcast(EInventoryChanged::Retrieved, InventorySlots[SlotIndex].GetStackId(), InventorySlots[SlotIndex].StackSize);

	if (RemainingStackSize <= 0)
	{
		// The whole stack is retrieved, invalidate this slot.
		InventorySlots[SlotIndex] = FInventoryItemStack();
	}

	return true;
}
