// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/CorrbolgInventoryEntryDefinition.h"

#include "Logging/CorrbolgLogChannels.h"

void UCorrbolgInventoryEntryDefinition::Init(const FGuid& Id, const FText& Name, const FText& Description, const FGameplayTag& Type, const TArray<UCorrbolgInventoryEntryFragment*>& Fragments)
{
    EntryId = Id;
    EntryName = Name;
    EntryDescription = Description;
    EntryType = Type;
    EntryFragments = Fragments;
}

void UCorrbolgInventoryEntryDefinition::Init(const UCorrbolgInventoryEntryDefinition* const EntryDefinition)
{
    if(!IsValid(EntryDefinition))
    {
        UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to initialize, EntryDefinition is invalid."), *FString(__FUNCTION__));
        return;
    }

    Init(EntryDefinition->GetId(), EntryDefinition->GetName(), EntryDefinition->GetDescription(), EntryDefinition->GetType(), EntryDefinition->GetFragments());
}

void UCorrbolgInventoryEntryDefinition::Reset()
{
    EntryId = FGuid();
    EntryName = FText();
    EntryDescription = FText();
    EntryType = FGameplayTag::EmptyTag;
    EntryFragments.Empty();
}

bool UCorrbolgInventoryEntryDefinition::IsDefault() const
{
    return  EntryId == FGuid() &&
            EntryName.IsEmpty() &&
            EntryDescription.IsEmpty() &&
            EntryType == FGameplayTag::EmptyTag &&
            EntryFragments.IsEmpty();
}

bool UCorrbolgInventoryEntryDefinition::IsStackable() const
{
    return EntryFragments.IsEmpty();
}
