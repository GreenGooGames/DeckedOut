// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryComponent.h"

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

FInventoryStackId UTartarusInventoryComponent::StoreEntry(const EInventoryType InventoryId, const int32 EntryId, const int32 StackSize)
{
	// Verify the given parameters.
	if (InventoryId == EInventoryType::MAX)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: InventoryType was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}
	
	if (EntryId == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: ItemId was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	if (StackSize <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: StackSize was  <= 0 !"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// TODO: Probably should not use positive and negatives to differentiate between stackable and non-stackable.
	const bool bIsStackableItem = EntryId > FTartarusHelpers::InvalidItemId;
	if (!bIsStackableItem && StackSize > 1)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: Trying to add multiples of a unique entry in the same stack!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Try to add the entry to the specified sub inventory.
	const FInventoryStackId StackId = SubInventories[InventoryId].AddEntry(EntryId, StackSize);
	if (!StackId.IsValid())
	{
		return FInventoryStackId();
	}
	
	OnInventoryChanged().Broadcast(EInventoryChanged::Stored, StackId, SubInventories[InventoryId].FindStack(StackId)->StackSize);

	return StackId;
}

bool UTartarusInventoryComponent::RetrieveEntry(const EInventoryType InventoryId, const int32 EntryId, const int32 StackSize)
{
	// Verify the given EntryId.
	if (EntryId == FTartarusHelpers::InvalidItemId)
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

const TArray<const FInventoryStack*> UTartarusInventoryComponent::GetOverviewMulti(const EInventoryType InventoryId, const int32 EntryId) const
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

bool UTartarusInventoryComponent::Contains(const EInventoryType InventoryId, const int32 EntryId) const
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
