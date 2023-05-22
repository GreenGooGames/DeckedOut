// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "System/TartarusHelpers.h"

#include "System/TartarusAsyncLoadData.h"

#include "TartarusInventoryComponent.generated.h"

DECLARE_EVENT_ThreeParams(UTartarusInventoryComponent, FInventoryEntryUpdate, EInventoryChanged /*ChangeType*/, FInventoryStackId /*StackId*/, int32 /*StackSize*/);
DECLARE_EVENT(UTartarusInventoryComponent, FInventoryContentUpdate);

class UTartarusContextAction;
class UTartarusItem;
class UTartarusSubInventoryData;

USTRUCT()
struct FGetSortItemDataRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FGetSortItemDataRequestInfo() {}
	FGetSortItemDataRequestInfo(const TArray<FPrimaryAssetId>& ItemIdsToLoad, FGameplayTag SubInventoryId);

public:
	TArray<FPrimaryAssetId> ItemIds;
	FGameplayTag InventoryId;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/*
	* Stores an Entry into the inventory.
	* Return: The StackId of the stack the Entry was added to.
	*/
	FInventoryStackId StoreEntry(const UTartarusItem* const Entry, const int32 StackSize);

	/*
	* Retrieves an entry from the inventory from a random matching stack.
	* Return: True if the entry was retrieved, False if the entry was not in the inventory or if more than the stored num of entries was requested.
	*/
	bool RetrieveEntry(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId, const int32 StackSize);

	/*
	* Retrieves an entry from the inventory from a specified stack.
	* Return: True if the entry was retrieved, False if the entry was not in the inventory or if more than the stored num of entries was requested.
	*/
	bool RetrieveEntry(const FInventoryStackId& StackId, const int32 StackSize);

	/*
	* Retrieves an overview of all the entries of a sub-inventory.
	* Return: Array representing the inventory slots.
	*/
	const TArray<FInventoryStack>& GetOverview(const FGameplayTag InventoryId) const;

	/*
	* Retrieves an overview of all entries matching the id.
	* Return: Data stored in the inventory of each slot that contains the given id.
	*/
	const TArray<const FInventoryStack*> GetOverviewMulti(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId) const;

	/*
	* Retrieves an overview of a single slot.
	* Return: Data stored in the slot.
	*/
	const FInventoryStack* GetOverviewSingle(const FInventoryStackId& StackId) const;

	/*
	* Retrieves the number of slots in the inventory that are available for storage.
	* Return: Number of available slots.
	*/
	int32 GetAvailableSlotCount(const FGameplayTag InventoryId) const;

	/*
	* Checks if the inventory contains an entry for the AssetId
	* Return: True, if an entry is found. False, if no entry is found.
	*/
	bool Contains(const FGameplayTag InventoryId, const FPrimaryAssetId EntryId) const;

	/*
	* Checks if the inventory contains an entry for the StackId.
	* Return: True, if an entry is found/ False, if no entry is found.
	*/
	bool Contains(const FInventoryStackId& StackId) const;

	/*
	* Event fired when an entry in the inventory is updated.
	* Return: The event fired when a change takes place.
	*/
	FInventoryEntryUpdate& OnInventoryEntryUpdated() { return InventoryEntryUpdateEvent; }

	/*
	* Event fired when multiple entries in the inventory have changed.
	* Return: The event fired when the change takes place.
	*/
	FInventoryContentUpdate& OnInventoryContentUpdated() { return InventoryContentUpdateEvent; }

	/*
	* Retreives the ID's of all sub inventories owned by this Inventory.
	* Return: Array of FGameplayTag with the ID of each owned inventory.
	*/
	TArray<FGameplayTag> GetSubInventoryIds() const;
	
	/*
	* Retreives the Name of a sub inventory.
	* Return: Localized Name of the inventory.
	*/
	FText GetSubInventoryName(const FGameplayTag InventoryId) const;

	TArray<UTartarusContextAction*> GetSubInventoryContextActions(const FGameplayTag InventoryId) const;

	/*
	* Sorts the ivnentory alphabetically.
	*/
	void SortInventory(const FGameplayTag InventoryId);

protected:
	UPROPERTY(EditDefaultsOnly)
		TArray<TObjectPtr<UTartarusSubInventoryData>> ToCreateSubInventories;

	void OnItemDataRecieved(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);

private:
	TMap<FGameplayTag, FSubInventory> SubInventories;
	FInventoryEntryUpdate InventoryEntryUpdateEvent;
	FInventoryContentUpdate InventoryContentUpdateEvent;

	TArray<FGetSortItemDataRequestInfo> DataRequests;
};
