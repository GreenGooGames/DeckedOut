// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/CorrbolgInventory.h"

#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "Inventory/CorrbolgInventorySettings.h"
#include "Logging/CorrbolgLogChannels.h"

void UCorrbolgInventory::ApplySettings(const UCorrbolgInventorySettings* const Settings)
{
    if (!IsValid(Settings))
    {
        UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to apply settings, Settings is invalid"), *FString(__FUNCTION__));
        return;
    }

    EntryFilter = Settings->GetFilter();
    bAreEntriesLimited = Settings->GetEntryLimit() >= -1 ? true : false;

    // If the entries are limited, initialize all inventory entries with an empty entry.
    if (bAreEntriesLimited)
    {
        for (int i = 0; i < Settings->GetEntryLimit(); i++)
        {
            UCorrbolgInventoryEntryDefinition* const DefaultDefinition = NewObject<UCorrbolgInventoryEntryDefinition>();
            DefaultDefinition->Reset();

            AddEntry(DefaultDefinition);
        }
    }
}

bool UCorrbolgInventory::StoreEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition)
{
    if (!IsValid(EntryDefinition))
    {
        UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to store entry, EntryDefinition is invalid."), *FString(__FUNCTION__));
        return false;
    }

    // Can the entry be stored inside of this inventory?
    if (!EntryFilter.MatchesTagExact(EntryDefinition->GetType()))
    {
        return false;
    }
    
    // Increase the stack count if the entry is stackable and exists.
    if (EntryDefinition->IsStackable())
    {
        FCorrbolgInventoryEntry* const StackableEntry = FindStackableEntry(EntryDefinition);
        if (StackableEntry != nullptr)
        {
            StackableEntry->StackSize++;
            return true;
        }
    }

    // If the inventory size is limited, replace a default entry.
    if (bAreEntriesLimited)
    {
        FCorrbolgInventoryEntry* const DefaultEntry = FindDefaultEntry();
        if (DefaultEntry != nullptr)
        {
            DefaultEntry->EntryDefinition = EntryDefinition;
            DefaultEntry->StackSize = 1;
            return true;
        }
    }

    AddEntry(EntryDefinition);

    return true;
}

UCorrbolgInventoryEntryDefinition* UCorrbolgInventory::RetrieveEntry(const FGuid& EntryId)
{
    // Find and copy the Entry definition.
    FCorrbolgInventoryEntry* const Entry = Entries.FindByPredicate([&EntryId](FCorrbolgInventoryEntry& Entry)
        {
            return Entry.EntryDefinition->GetId() == EntryId;
        });

    if (Entry == nullptr)
    {
        return nullptr;
    }

    UCorrbolgInventoryEntryDefinition* const RemovedEntryDefinition = NewObject<UCorrbolgInventoryEntryDefinition>();
    RemovedEntryDefinition->Init(Entry->EntryDefinition);

    // Decrease the stacksize and remove the entry if the Stack reaches 0.
    Entry->StackSize--;

    if (Entry->StackSize <= 0)
    {
        if (bAreEntriesLimited)
        {
            Entry->Reset();
        }
        else
        {
            Entries.RemoveSingle(*Entry);
        }
    }

    return RemovedEntryDefinition;
}

const TArray<FCorrbolgInventoryEntry>& UCorrbolgInventory::GetEntries() const
{
    return Entries;
}

bool UCorrbolgInventory::AddEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition)
{
    if (!IsValid(EntryDefinition))
    {
        UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to add entry, EntryDefinition is invalid."), *FString(__FUNCTION__));
        return false;
    }

    FCorrbolgInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.EntryDefinition = EntryDefinition;
    NewEntry.StackSize++;

    return true;
}

FCorrbolgInventoryEntry* UCorrbolgInventory::FindStackableEntry(const UCorrbolgInventoryEntryDefinition* const EntryDefinition)
{
    FCorrbolgInventoryEntry* const StackableEntry = Entries.FindByPredicate([&EntryDefinition](const FCorrbolgInventoryEntry& Entry)
        {
            if (Entry.EntryDefinition == nullptr || !Entry.EntryDefinition->IsStackable())
            {
                return false;
            }
    
            return Entry.EntryDefinition == EntryDefinition;
        });
    
    return StackableEntry;
}

FCorrbolgInventoryEntry* UCorrbolgInventory::FindDefaultEntry()
{
    FCorrbolgInventoryEntry* const DefaultEntry = Entries.FindByPredicate([](const FCorrbolgInventoryEntry& Entry)
        {
            if (Entry.EntryDefinition->IsDefault())
            {
                return true;
            }

            return false;
        });

    return DefaultEntry;
}
