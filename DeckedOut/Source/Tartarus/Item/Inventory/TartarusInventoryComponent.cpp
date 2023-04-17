// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryComponent.h"

#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"

// Called when the game starts
void UTartarusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ToCreateSubInventories.Num() <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create subinventory entries: No subinventories were set to be created!"), *FString(__FUNCTION__));
		return;
	}

	for (const EInventoryType InventoryId : TEnumRange<EInventoryType>())
	{
		SubInventories.Add(InventoryId, FSubInventory(InventoryId, NumberOfSlots));
	}
}

// TODO: Instead of using UTartarusItem, create a UTartarusInventoryEntry type. Items can then inherit this class.
FInventoryStackId UTartarusInventoryComponent::StoreEntry(const UTartarusItem* const Entry, const int32 StackSize)
{
	// Verify the entry is valid.
	if (!IsValid(Entry))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: Entry was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Verify the given parameters.
	if (Entry->InventoryType == EInventoryType::MAX)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: InventoryType was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}
	
	// Verify that the asset has a valid Id.
	if (!Entry->GetPrimaryAssetId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: ItemId was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Verify that the stacksize is positive.
	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: StackSize was  <= 0 !"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}
	
	// Ensure that the entry is stackable if storing multiples.
	if (!Entry->bIsStackable && StackSize > 1)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: Trying to add multiples of a unique entry in the same stack!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Try to add the entry to the specified sub inventory.
	const FInventoryStackId StackId = SubInventories[Entry->InventoryType].AddEntry(Entry->GetPrimaryAssetId(), Entry->bIsStackable, StackSize);
	if (!StackId.IsValid())
	{
		return FInventoryStackId();
	}
	
	OnInventoryChanged().Broadcast(EInventoryChanged::Stored, StackId, SubInventories[Entry->InventoryType].FindStack(StackId)->StackSize);

	return StackId;
}

bool UTartarusInventoryComponent::RetrieveEntry(const EInventoryType InventoryId, const FPrimaryAssetId EntryId, const int32 StackSize)
{
	// Verify the given EntryId.
	if (!EntryId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the entry: EntryId was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the stack that contains the Entry.
	const FInventoryStackId* const StackId = SubInventories[InventoryId].FindStackId(EntryId);
	if (StackId == nullptr)
	{
		return false;
	}

	return RetrieveEntry(*StackId, StackSize);
}

bool UTartarusInventoryComponent::RetrieveEntry(const FInventoryStackId& StackId, const int32 StackSize)
{
	// Verify the given StackId.
	if (!StackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the entry: EntryId was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Retrieve the Entry from the inventory.
	const bool bHasRemoved = SubInventories[StackId.GetInventoryId()].RemoveEntry(StackId, StackSize);
	if (!bHasRemoved)
	{
		return false;
	}

	const FInventoryStack* const EntryStack = SubInventories.Contains(StackId.GetInventoryId()) ? SubInventories[StackId.GetInventoryId()].FindStack(StackId) : nullptr;
	const int32 RemainingStackSize = EntryStack != nullptr ? EntryStack->StackSize : 0;

	OnInventoryChanged().Broadcast(EInventoryChanged::Retrieved, StackId, RemainingStackSize);

	return true;
}

const TArray<FInventoryStack>& UTartarusInventoryComponent::GetOverview(const EInventoryType InventoryId) const
{ 
	check(SubInventories.Num() > 0);
	check(SubInventories.Contains(InventoryId));

	return SubInventories[InventoryId].GetContents();
}

const TArray<const FInventoryStack*> UTartarusInventoryComponent::GetOverviewMulti(const EInventoryType InventoryId, const FPrimaryAssetId EntryId) const
{
	TArray<const FInventoryStack*> MatchingItemSlots = TArray<const FInventoryStack*>();
	
	for (const FInventoryStack& Stack : SubInventories[InventoryId].GetContents())
	{
		if (Stack.GetEntryId() == EntryId)
		{
			MatchingItemSlots.Add(&Stack);
		}
	}

	return MatchingItemSlots;
}

const FInventoryStack* UTartarusInventoryComponent::GetOverviewSingle(const FInventoryStackId& StackId) const
{
	return SubInventories[StackId.GetInventoryId()].FindStack(StackId);
}

int32 UTartarusInventoryComponent::GetAvailableSlotCount(const EInventoryType InventoryId) const
{
	return GetOverviewMulti(InventoryId, FTartarusHelpers::InvalidItemId).Num();
}

bool UTartarusInventoryComponent::Contains(const EInventoryType InventoryId, const FPrimaryAssetId EntryId) const
{
	// Get the stack that contains the Entry.
	const FInventoryStackId* const StackId = SubInventories[InventoryId].FindStackId(EntryId);
	if (StackId == nullptr)
	{
		return false;
	}
	
	return Contains(*StackId);
}

bool UTartarusInventoryComponent::Contains(const FInventoryStackId& StackId) const
{
	for (const FInventoryStack& Stack : SubInventories[StackId.GetInventoryId()].GetContents())
	{
		if (Stack.GetStackId() == StackId)
		{
			return true;
		}
	}

	return false;
}
