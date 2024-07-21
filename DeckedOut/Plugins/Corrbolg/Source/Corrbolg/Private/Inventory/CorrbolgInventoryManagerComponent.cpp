// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/CorrbolgInventoryManagerComponent.h"

#include "Inventory/CorrbolgInventory.h"
#include "Inventory/CorrbolgInventoryEntryDefinition.h"
#include "Inventory/CorrbolgInventorySettings.h"
#include "Logging/CorrbolgLogChannels.h"

void UCorrbolgInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupInventories();
}

bool UCorrbolgInventoryManagerComponent::StoreEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition) const
{
	if (!IsValid(EntryDefinition))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to store entry, no entry definition provided."), *FString(__FUNCTION__));
		return false;
	}

	UCorrbolgInventory* const Inventory = FindInventoryForFilter(EntryDefinition->GetType());
	if (!IsValid(Inventory))
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to store entry, no inventory found that could store the entry."), *FString(__FUNCTION__));
		return false;
	}
	
	const bool bIsItemStored = Inventory->StoreEntry(EntryDefinition);
	
	return bIsItemStored;
}

UCorrbolgInventoryEntryDefinition* UCorrbolgInventoryManagerComponent::RetrieveEntry(const FGuid& EntryId) const
{
	for (UCorrbolgInventory* const Inventory : Inventories)
	{
		UCorrbolgInventoryEntryDefinition* const RetrievedEntry = Inventory->RetrieveEntry(EntryId);
		if (IsValid(RetrievedEntry))
		{
			return RetrievedEntry;
		}
	}
	
	return nullptr;
}

const TArray<TSoftObjectPtr<UCorrbolgInventorySettings>>& UCorrbolgInventoryManagerComponent::GetInventorySettings() const
{
	return InventorySettings;
}

const UCorrbolgInventory* UCorrbolgInventoryManagerComponent::FindInventoryForFilterConst(const FGameplayTag& Filter) const
{
	return FindInventoryForFilter(Filter);
}

void UCorrbolgInventoryManagerComponent::SetupInventories()
{
	if (InventorySettings.IsEmpty())
	{
		UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to setup inventory, InventorySettings is empty."), *FString(__FUNCTION__));
		return;
	}

	for (const TSoftObjectPtr<UCorrbolgInventorySettings>& SoftSettings : InventorySettings)
	{
		// TODO: Koen - Load the Settings Async.
		UCorrbolgInventorySettings* const Settings = Cast<UCorrbolgInventorySettings>(SoftSettings.LoadSynchronous());
		if (!IsValid(Settings))
		{
			UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to setup inventory, Settings is invalid."), *FString(__FUNCTION__));
			continue;
		}

		UCorrbolgInventory* const Inventory = NewObject<UCorrbolgInventory>(this);
		if (!IsValid(Inventory))
		{
			UE_LOG(LogCorrbolg, Log, TEXT("%s: Failed to setup inventory, could not create an inventory."), *FString(__FUNCTION__));
			continue;
		}

		Inventory->ApplySettings(Settings);
		Inventories.Add(Inventory);
	}
}

UCorrbolgInventory* UCorrbolgInventoryManagerComponent::FindInventoryForFilter(const FGameplayTag& Filter) const
{
	for (UCorrbolgInventory* const Inventory : Inventories)
	{
		if (Inventory->GetEntryFilter().MatchesTagExact(Filter))
		{
			return Inventory;
		}
	}

	return nullptr;
}
