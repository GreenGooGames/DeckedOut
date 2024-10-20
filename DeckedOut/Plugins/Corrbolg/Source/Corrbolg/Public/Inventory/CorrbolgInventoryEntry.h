// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

#include "CorrbolgInventoryEntry.generated.h"

class UCorrbolgInventoryEntryDefinition;

/**
 * Holds a representation of a stored entry in an inventory.
 */
USTRUCT()
struct CORRBOLG_API FCorrbolgInventoryEntry
{
	GENERATED_BODY()

public:
	/** Total entries stored.*/
	int StackSize = 0;

	/** The definition of the entry.*/
	UPROPERTY()
	TObjectPtr<UCorrbolgInventoryEntryDefinition> EntryDefinition = nullptr;

	/**
	* Equals operator to compare FCorrbolgInventoryEntry with FCorrbolgInventoryEntry.
	* @param rhs: The other instance to compare against.
	* @return: True of the instances hold the same data, false if there are differences.
	*/
	bool operator==(const FCorrbolgInventoryEntry& rhs) const
	{
		return StackSize == rhs.StackSize && EntryDefinition == rhs.EntryDefinition;
	}

	void Reset();
};
