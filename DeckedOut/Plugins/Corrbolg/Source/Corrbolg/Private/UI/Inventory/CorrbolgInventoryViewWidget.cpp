// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryViewWidget.h"

#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "Inventory/CorrbolgInventory.h"
#include "Inventory/CorrbolgInventorySettings.h"
#include "UI/Inventory/CorrbolgInventoryViewListItem.h"
#include "Logging/CorrbolgLogChannels.h"
#include "CommonTileView.h"
#include "Utilities/CorrbolgUtilities.h"

void UCorrbolgInventoryViewWidget::Init(const UCorrbolgInventorySettings& Settings)
{
	Filter = Settings.GetFilter();

	Refresh();
}

void UCorrbolgInventoryViewWidget::Refresh() const
{
	UCorrbolgInventoryManagerComponent* const InventoryManager = FCorrbolgUtilities::FindInventoryComponent(GetOwningPlayer());
	if (!IsValid(InventoryManager))
	{
		return;
	}

	const UCorrbolgInventory* const Inventory = InventoryManager->FindInventoryForFilterConst(Filter);
	if (!IsValid(Inventory))
	{
		return;
	}

	TileView->ClearListItems();

	// Get all entries of the inventory and initialize the data to display.
	const TArray<FCorrbolgInventoryEntry>& InventoryEntries = Inventory->GetEntries();
	for (const FCorrbolgInventoryEntry& Entry : InventoryEntries)
	{
		UCorrbolgInventoryViewListItem* const ListItem = NewObject<UCorrbolgInventoryViewListItem>(TileView);
		if (!IsValid(ListItem))
		{
			continue;
		}

		ListItem->Init(Entry);
		TileView->AddItem(ListItem);
	}
}

