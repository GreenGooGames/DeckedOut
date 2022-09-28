// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Components/Inventory/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	StoredItems.SetNum(NumInventorySlots, true);
}

bool UInventoryComponent::RetrieveItems(const int32 ItemId, const int32 Amount, int32& OutRetrievedAmount, TMap<FString, ItemUniqueDataType>& OutUniqueData)
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
			OutUniqueData = StoredItems[Index].UniqueData;

			// Check if the inventory is empty for the requested item.
			if (StoredItems[Index].StackSize <= 0)
			{
				// Zero the entry.
				StoredItems[Index] = FItemData_Inventory();
			}

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::StoreItem(const FItemData& ItemData, const TMap<FString, ItemUniqueDataType>& UniqueData, const int32 Amount)
{
	int32 IndexToStore = INDEX_NONE;

	if (ItemData.IsDataValid() && Amount > 0)
	{
		// Check if the item is stackable and exist in the inventory.
		const bool bIsUniqueItem = UniqueData.Num() > 0;	// Unique items cannot be stacked.

		if (!bIsUniqueItem)
		{
			IndexToStore = FindIndexOfStoredItemData(ItemData.Id);
		}

		// Store the item in the inventory.
		if (IndexToStore == INDEX_NONE)
		{
			// Find the first available slot to store the Item.
			IndexToStore = FindFirstAvailableSlot();

			// Check that a valid index has been found, otherwise the inventory is full!
			if (IndexToStore != INDEX_NONE)
			{
				StoredItems[IndexToStore] = FItemData_Inventory(ItemData, Amount);
			}
		}
		else
		{
			StoredItems[IndexToStore].StackSize += Amount;
		}
	}

	// If an index has been found, assume the item has been stored in the inventory.
	const bool bIsIndexFound = IndexToStore != INDEX_NONE;

	return bIsIndexFound;
}

const TArray<FItemData_Inventory>& UInventoryComponent::GetStoredItems() const
{
	return StoredItems;
}

int32 UInventoryComponent::FindIndexOfStoredItemData(const int32 ItemId) const
{
	const int32 Index = StoredItems.IndexOfByPredicate([ItemId](const FItemData_Inventory& StoredItemData)
	{
		return StoredItemData.Id == ItemId;
	});

	return Index;
}

int32 UInventoryComponent::FindFirstAvailableSlot() const
{
	const int32 Index = StoredItems.IndexOfByPredicate([](const FItemData_Inventory& StoredItemData)
		{
			return StoredItemData.Id == INVALID_ItemId;
		});

	return Index;

}