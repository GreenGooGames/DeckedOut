// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusItemSelectionWidget.h"

#include "CommonTileView.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "Logging/TartarusLogChannels.h"

void UTartarusItemSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Populate();
}

void UTartarusItemSelectionWidget::Populate()
{
	TArray<UTartarusInventorySlotWidgetData*> SlotsData;

	if (TileView->GetNumItems() != 0)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to Populate TileView: The Tileview is already populated!"), *FString(__FUNCTION__));
		return;
	}

	for (int i = 0; i < NumEntries; i++)
	{
		// If a ListItem exist to assign to the InventoryItem, retrieve and update. Else Create a new ListItem and then set the data.
		UTartarusInventorySlotWidgetData* const ListItem = CreateListItemData();
		if (!IsValid(ListItem))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to Populate TileView: Could not create a list entry!"), *FString(__FUNCTION__));
			continue;
		}
	}
}

UTartarusInventorySlotWidgetData* UTartarusItemSelectionWidget::CreateListItemData()
{
	// Create an instance to save all the data in.
	UTartarusInventorySlotWidgetData* const SlotData = NewObject<UTartarusInventorySlotWidgetData>();
	if (!IsValid(SlotData))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of InventorySlotWidgetData!"), *FString(__FUNCTION__));
		return nullptr;
	}

	// Add the data to the Tileview to display.
	TileView->AddItem(SlotData);

	return SlotData;
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusItemSelectionWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(TileView))
	{
		return nullptr;
	}

	return TileView;
}
#pragma endregion
