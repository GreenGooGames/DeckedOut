// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inventory/TartarusInventoryComponent.h"

#include "Item/Inventory/TartarusSubInventoryData.h"
#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#include "Engine/World.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"

// Called when the game starts
void UTartarusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ToCreateSubInventories.Num() <= 0)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create subinventory entries: No subinventories were set to be created!"), *FString(__FUNCTION__));
		return;
	}

	for (const TObjectPtr<UTartarusSubInventoryData>& SubInventoryData : ToCreateSubInventories)
	{
		if (!IsValid(SubInventoryData))
		{
			UE_LOG(LogTartarus, Error, TEXT("%s: Unable to create subinventory entries: The SubInventory to create is null!"), *FString(__FUNCTION__));
			continue;
		}

		SubInventories.Add(SubInventoryData->GetType(), FSubInventory(SubInventoryData->GetType(), SubInventoryData->GetNumberOfSlots()));
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

	FInventoryStackId StackId = StoreEntry(Entry->InventoryId, Entry->GetPrimaryAssetId(), Entry->bIsStackable, StackSize);

	return StackId;
}

FInventoryStackId UTartarusInventoryComponent::StoreEntry(const FGameplayTag& EntryInventoryId, const FPrimaryAssetId& EntryId, const bool bIsEntryStackAble, const int32 StackSize)
{
	// Verify the given parameters.
	if (!EntryInventoryId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: InventoryId was invalid!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Verify that the asset has a valid Id.
	if (!EntryId.IsValid())
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
	if (!bIsEntryStackAble && StackSize > 1)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to store the entry: Trying to add multiples of a unique entry in the same stack!"), *FString(__FUNCTION__));
		return FInventoryStackId();
	}

	// Try to add the entry to the specified sub inventory.
	const FInventoryStackId StackId = SubInventories[EntryInventoryId].AddEntry(EntryId, bIsEntryStackAble, StackSize);
	if (!StackId.IsValid())
	{
		return FInventoryStackId();
	}

	OnInventoryEntryUpdated().Broadcast(EInventoryChanged::Stored, StackId, SubInventories[EntryInventoryId].FindStack(StackId)->StackSize);

	return StackId;
}

bool UTartarusInventoryComponent::RetrieveEntry(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId, const int32 StackSize)
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the entry: Could not find the entry!"), *FString(__FUNCTION__));
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
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve the entry: Entry is not removed!"), *FString(__FUNCTION__));
		return false;
	}

	const FInventoryStack* const EntryStack = SubInventories.Contains(StackId.GetInventoryId()) ? SubInventories[StackId.GetInventoryId()].FindStack(StackId) : nullptr;
	const int32 RemainingStackSize = EntryStack != nullptr ? EntryStack->StackSize : 0;

	OnInventoryEntryUpdated().Broadcast(EInventoryChanged::Retrieved, StackId, RemainingStackSize);

	return true;
}

const TArray<FInventoryStack>& UTartarusInventoryComponent::GetOverview(const FGameplayTag InventoryId) const
{ 
	check(SubInventories.Num() > 0);
	check(SubInventories.Contains(InventoryId));

	return SubInventories[InventoryId].GetContents();
}

const TArray<const FInventoryStack*> UTartarusInventoryComponent::GetOverviewMulti(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId) const
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

int32 UTartarusInventoryComponent::GetAvailableSlotCount(const FGameplayTag InventoryId) const
{
	return GetOverviewMulti(InventoryId, FTartarusHelpers::InvalidItemId).Num();
}

bool UTartarusInventoryComponent::Contains(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId) const
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

TArray<FGameplayTag> UTartarusInventoryComponent::GetSubInventoryIds() const
{
	TArray<FGameplayTag> Ids;

	for (const auto& SubInventory : SubInventories)
	{
		Ids.Add(SubInventory.Key);
	}

	return Ids;
}

FText UTartarusInventoryComponent::GetSubInventoryName(const FGameplayTag InventoryId) const
{
	for (const auto& SubInventoryData : ToCreateSubInventories)
	{
		if (SubInventoryData->GetType() != InventoryId)
		{
			continue;
		}

		return SubInventoryData->GetName();
	}

	return FText();
}

void UTartarusInventoryComponent::SortInventory(const FGameplayTag InventoryId)
{
	if (!SubInventories.Contains(InventoryId))
	{
		return;
	}

	// Get all entries.
	FSubInventory& SubInventory = SubInventories[InventoryId];
	const TArray<FInventoryStack>& Content = SubInventory.GetContents();

	// Load the names for each entry.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to treasure: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return;
	}

	TArray<FPrimaryAssetId> ItemIds;
	for (const FInventoryStack& Stack : Content)
	{
		ItemIds.Add(Stack.GetEntryId());
	}

	FGetItemDataRequestCompletedEvent OnCompletedEvent;
	OnCompletedEvent.AddUObject(this, &UTartarusInventoryComponent::OnItemDataRecieved);

	FGuid AsyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnCompletedEvent);

	FGetSortItemDataRequestInfo Request = FGetSortItemDataRequestInfo(ItemIds, InventoryId);
	Request.SetASyncLoadRequestId(AsyncRequestId);

	DataRequests.Add(Request);
}

void UTartarusInventoryComponent::OnItemDataRecieved(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FGetSortItemDataRequestInfo* const CurrentRequest = DataRequests.FindByPredicate([&ASyncLoadRequestId](const FGetSortItemDataRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Sort Ivnentory failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	SubInventories[CurrentRequest->InventoryId].Sort(ItemsData);
	OnInventoryContentUpdated().Broadcast();
}

FGetSortItemDataRequestInfo::FGetSortItemDataRequestInfo(const TArray<FPrimaryAssetId>& ItemIdsToLoad, FGameplayTag SubInventoryId)
{
	RequestId = FGuid::NewGuid();
	ItemIds = ItemIdsToLoad;
	InventoryId = SubInventoryId;
}
