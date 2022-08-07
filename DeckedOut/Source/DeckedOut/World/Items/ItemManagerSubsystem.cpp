// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/World/Items/ItemManagerSubsystem.h"

void UItemManagerSubsystem::SetItemsDataTable(const TSoftObjectPtr<UDataTable>& InItemsDataTable)
{
	if (IsValid(InItemsDataTable.LoadSynchronous()))
	{
		ItemsDataTable = InItemsDataTable.Get();
	}
}

TSoftObjectPtr<AItem> UItemManagerSubsystem::SpawnItem(const int32 ItemId, const FTransform& SpawnTransform)
{
	return SpawnItem(ItemId, SpawnTransform, TMap<FString, ItemUniqueDataType>());
}

TSoftObjectPtr<AItem> UItemManagerSubsystem::SpawnItem(const int32 ItemId, const FTransform& SpawnTransform, const TMap<FString, ItemUniqueDataType>& ItemData)
{
	// [Koen Goossens] TODO: Async spawning
	// [Koen Goossens] TODO: This approach of loading each DataAsset to check if the ID is the same is not scaleable!
	// 
	// Load the datatable if not loaded.
	if (const UDataTable* const ItemsDataTableRaw = ItemsDataTable.Get())
	{
		// Retrieve all rows in the datatable.
		FString ContextString = "";
		TArray<FItemsTableRow*> DataTableRows;

		ItemsDataTableRaw->GetAllRows<FItemsTableRow>(ContextString, DataTableRows);

		// Find the correct UDataAsset for the requested item.
		const UItemDataAsset* ItemDataAsset = nullptr;
		
		for (const FItemsTableRow* const ItemTableRow : DataTableRows)
		{
			ItemDataAsset = ItemTableRow->ItemDataAsset.LoadSynchronous();

			if (IsValid(ItemDataAsset))
			{
				if (ItemDataAsset->ItemData.Id == ItemId)
				{
					break;
				}

				ItemDataAsset = nullptr;
			}
		}

		// Spawn an instance of the Blueprint reference in FItemData.
		if (IsValid(ItemDataAsset) && ItemDataAsset->ItemData.IsDataValid())
		{
			UClass* const ItemClass = ItemDataAsset->ItemData.Blueprint;
			AItem* const SpawnedItem = GetWorld()->SpawnActor<AItem>(ItemClass, SpawnTransform);

			if (SpawnedItem)
			{
				SpawnedItem->SetItemData(ItemDataAsset->ItemData);
				SpawnedItem->WriteUniqueData(ItemData);

				SpawnedItems.Add(SpawnedItem);
			}

			return SpawnedItem;
		}
	}

	return nullptr;
}

bool UItemManagerSubsystem::DespawnItem(const TSoftObjectPtr<AItem>& Item)
{
	AItem* const ItemToDespawn = Item.Get();

	if (IsValid(ItemToDespawn))
	{
		SpawnedItems.Remove(ItemToDespawn);
		ItemToDespawn->Destroy();

		return true;
	}

	return false;
}
