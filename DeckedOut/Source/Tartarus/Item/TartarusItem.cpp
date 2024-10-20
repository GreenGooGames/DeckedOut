// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TartarusItem.h"

#include "Inventory/CorrbolgInventoryEntryDefinition.h"

UCorrbolgInventoryEntryDefinition* UTartarusItem::CreateEntryDefinition() const
{
    TArray<UCorrbolgInventoryEntryFragment*> EntryFragments = TArray<UCorrbolgInventoryEntryFragment*>();

    // Create a UI fragment.

    UCorrbolgInventoryEntryDefinition* const Definition = NewObject<UCorrbolgInventoryEntryDefinition>();
    Definition->Init(Id, Name, Description, ItemTypeTag, EntryFragments);

    return Definition;
}
