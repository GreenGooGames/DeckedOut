// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"

#include "CorrbolgInventoryEntry.h"

#include "CorrbolgInventory.generated.h"

class UCorrbolgInventoryEntryDefinition;
class UCorrbolgInventorySettings;

/**
 * Holds a collection of entries and provides functionality to manipulate them.
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventory : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	* Retrieves the filter to store entries.
	* @return: The filter on this inventory.
	*/
	const FGameplayTag& GetEntryFilter() const {return EntryFilter;}

	/**
	* Applies the given settings to this inventory.
	* @param Settings: The setting to apply.
	*/
	void ApplySettings(const UCorrbolgInventorySettings* const Settings);

	/**
	* Stores an entry in the inventory.
	* @param EntryDefinition: The definition of the entry.
	* @return: True if the entry was stored, false if the entry could not be stored.
	*/
	bool StoreEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition);

	/**
	* Retrieves an entry from the inventory.
	* @param EntryId: The id of the entry to retrieve.
	* @return: The definition of the retrieved entry, nullptr if no entry was found.
	*/
	UCorrbolgInventoryEntryDefinition* RetrieveEntry(const FGuid& EntryId);

	/** Retrieves a list of all entries. */
	const TArray<FCorrbolgInventoryEntry>& GetEntries() const;

private:
	/** Filter to only allow entries with matching tags.*/
	FGameplayTag EntryFilter = FGameplayTag();

	/** Wether this can only have a limited amount of entries.*/
	bool bAreEntriesLimited = false;

	/**The entries stored in the inventory.*/
	UPROPERTY()
	TArray<FCorrbolgInventoryEntry> Entries = TArray<FCorrbolgInventoryEntry>();

	/**
	* Creates a new entry using the EntryDefinition and adds it to the stored entries collection.
	* @param EntryDefinition: The definition of the entry.
	* @return: True if the entry was added, false if the entry was not added.
	*/
	bool AddEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition);

	/**
	* Finds an entry that can be stacked.
	* @param EntryDefinition: The definition of the entry to search for.
	* @return: A stackable entry of the provided EntryDefinition, nullptr if none was found.
	*/
	FCorrbolgInventoryEntry* FindStackableEntry(const UCorrbolgInventoryEntryDefinition* const EntryDefinition);

	/**
	* Find a default entry to overwrite.
	* @param EntryDefinition: The definition of the entry to search for.
	* @return: A stackable entry of the provided EntryDefinition, nullptr if none was found.
	*/
	FCorrbolgInventoryEntry* FindDefaultEntry(const UCorrbolgInventoryEntryDefinition* const EntryDefinition);

};
