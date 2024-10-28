// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionDelete.h"
#include "Utilities/CorrbolgUtilities.h"
#include "Engine/World.h"
#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "UI/Inventory/CorrbolgInventoryViewListItem.h"
#include "Inventory/CorrbolgInventoryEntry.h"
#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "CommonUserWidget.h"

void UCorrbolgContextActionDelete::Execute(const FCorrbolgContextActionData& Context)
{
	if (!Context.Object.IsValid())
	{
		return;
	}

	// Get the inventory.
	UCorrbolgInventoryManagerComponent* const InventoryComponent = FCorrbolgUtilities::FindInventoryComponent(GetWorld()->GetFirstPlayerController());

	// Remove the items represented by the context object from the inventory.
	UCorrbolgInventoryViewListItem* const ListItem = Cast<UCorrbolgInventoryViewListItem>(Context.Object);
	const FCorrbolgInventoryEntry* const InventoryEntry = ListItem->GetInventoryEntry();

	for (int i = 0; i < InventoryEntry->StackSize; i++)
	{
		InventoryComponent->RetrieveEntry(InventoryEntry->EntryDefinition->GetId());
	}

	UCommonUserWidget* const Widget = Cast<UCommonUserWidget>(Context.Widget.Get());
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}
