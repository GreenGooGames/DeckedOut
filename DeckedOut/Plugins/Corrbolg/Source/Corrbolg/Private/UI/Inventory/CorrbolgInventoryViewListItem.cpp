// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryViewListItem.h"

#include "Inventory/CorrbolgInventoryEntry.h"
#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "Inventory/Fragments/CorrbolgInventoryUIFragment.h"

void UCorrbolgInventoryViewListItem::Init(const FCorrbolgInventoryEntry& Entry)
{
	UCorrbolgInventoryUIFragment* UIFragment = nullptr;
	const bool bFoundUIFragment = Entry.EntryDefinition->GetFragments().FindItemByClass<UCorrbolgInventoryUIFragment>(&UIFragment);

	if (bFoundUIFragment == false)
	{
		return;
	}

	StackSize = Entry.StackSize;
	Texture = UIFragment->Image;
}
