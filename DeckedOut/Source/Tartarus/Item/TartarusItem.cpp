// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TartarusItem.h"

#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "Inventory/Fragments/CorrbolgInventoryUIFragment.h"

TObjectPtr<UCorrbolgInventoryEntryDefinition> UTartarusItem::CreateEntryDefinition() const
{
    // Create a UI fragment.
    UCorrbolgInventoryUIFragment* const UIFragment = NewObject<UCorrbolgInventoryUIFragment>();
    UIFragment->Image = DisplayTexture;

    TArray<UCorrbolgInventoryEntryFragment*> EntryFragments = TArray<UCorrbolgInventoryEntryFragment*>();
    EntryFragments.Add(UIFragment);

    TObjectPtr<UCorrbolgInventoryEntryDefinition> const Definition = NewObject<UCorrbolgInventoryEntryDefinition>();
    Definition->Init(Id, Name, Description, ItemTypeTag, EntryFragments);

    return Definition;
}
