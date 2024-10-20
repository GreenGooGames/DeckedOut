// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/CorrbolgInventoryEntry.h"

#include "Inventory/CorrbolgInventoryEntryDefinition.h"

void FCorrbolgInventoryEntry::Reset()
{
	StackSize = 0;
	EntryDefinition->Reset();
}
