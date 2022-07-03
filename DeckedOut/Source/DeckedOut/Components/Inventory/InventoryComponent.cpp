// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Components/Inventory/InventoryComponent.h"

FStoredItemData::FStoredItemData(const FItemData& ItemData, const int32 InStackSize)
{
	ItemId = ItemData.Id;
	Name = ItemData.Name;
	Description = ItemData.Description;
	DisplayTexture = ItemData.DisplayTexture;

	StackSize = InStackSize;
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UInventoryComponent::RetrieveItems(const int32 ItemId, const int32 Amount, int32& OutRetrievedAmount)
{
	// Ensure the OutValues are Zero'ed.
	OutRetrievedAmount = 0;

	// Search if the requested item exist in the inventory.
	const int32 Index = FindIndexOfStoredItemData(ItemId);

	if (Index != INDEX_NONE)
	{
		// Get how much of the requested amount can be retrieved, clamp if needed.
		OutRetrievedAmount = FMath::Clamp(Amount, 0, StoredItems[Index].StackSize);

		// Check if any will be retrieved.
		if (OutRetrievedAmount > 0)
		{
			StoredItems[Index].StackSize -= OutRetrievedAmount;

			// Check if the inventory is empty for the requested item.
			if (StoredItems[Index].StackSize <= 0)
			{
				// Remove the entry, keep order.
				StoredItems.RemoveAt(Index, 1, true);
			}

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::StoreItem(const FItemData& ItemData, const int32 Amount)
{
	if (ItemData.IsDataValid() && Amount > 0)
	{
		// Search if the requested item exist in the inventory.
		const int32 Index = FindIndexOfStoredItemData(ItemData.Id);

		// If the item exist increase the stack size otherwise, create a new entry.
		if (Index != INDEX_NONE)
		{
			StoredItems[Index].StackSize += Amount;
		}
		else
		{
			StoredItems.Add(FStoredItemData(ItemData, Amount));
		}

		return true;
	}

	return false;
}

int32 UInventoryComponent::FindIndexOfStoredItemData(const int32 ItemId) const
{
	const int32 Index = StoredItems.IndexOfByPredicate([ItemId](const FStoredItemData& StoredItemData)
	{
		return StoredItemData.ItemId == ItemId;
	});

	return Index;
}