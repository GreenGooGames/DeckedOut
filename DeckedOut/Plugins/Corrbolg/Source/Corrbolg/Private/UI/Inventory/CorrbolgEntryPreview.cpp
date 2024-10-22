// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgEntryPreview.h"

#include "CommonTextBlock.h"

#include "UI/Inventory/CorrbolgInventoryViewListItem.h"
#include "Inventory/CorrbolgInventoryEntry.h"
#include "Inventory/CorrbolgInventoryEntryDefinition.h"

void UCorrbolgEntryPreview::SetPreviewEntry(const UCorrbolgInventoryViewListItem* const ViewListItem)
{
	if (!IsValid(ViewListItem))
	{
		EntryNameText->SetText(FText());
		EntryDescriptionText->SetText(FText());
		return;
	}

	EntryNameText->SetText(ViewListItem->GetInventoryEntry()->EntryDefinition->GetName());
	EntryDescriptionText->SetText(ViewListItem->GetInventoryEntry()->EntryDefinition->GetDescription());
}
