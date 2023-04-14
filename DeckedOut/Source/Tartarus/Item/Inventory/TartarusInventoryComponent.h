// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "System/TartarusHelpers.h"

#include "TartarusInventoryComponent.generated.h"

DECLARE_EVENT_ThreeParams(UTartarusInventoryComponent, FInventoryChanged, EInventoryChanged /*ChangeType*/, FInventoryStackId /*StackId*/, int32 /*StackSize*/);

class UTartarusItem;

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
	bool RetrieveEntry(const EInventoryType InventoryId, const FPrimaryAssetId EntryId, const int32 StackSize);

	/*
	* Retrieves an entry from the inventory from a specified stack.
	* Return: True if the entry was retrieved, False if the entry was not in the inventory or if more than the stored num of entries was requested.
	*/
	bool RetrieveEntry(const FInventoryStackId& StackId, const int32 StackSize);

	/*
	* Retrieves an overview of all the entries of a sub-inventory.
	* Return: Array representing the inventory slots.
	*/
	const TArray<FInventoryStack>& GetOverview(const EInventoryType InventoryId) const;

	/*
	* Retrieves an overview of all entries matching the id.
	* Return: Data stored in the inventory of each slot that contains the given id.
	*/
	const TArray<const FInventoryStack*> GetOverviewMulti(const EInventoryType InventoryId, const FPrimaryAssetId EntryId) const;

	/*
	* Retrieves an overview of a single slot.
	* Return: Data stored in the slot.
	*/
	const FInventoryStack* GetOverviewSingle(const FInventoryStackId& StackId) const;

	/*
	* Retrieves the number of slots in the inventory that are available for storage.
	* Return: Number of available slots.
	*/
	int32 GetAvailableSlotCount(const EInventoryType InventoryId) const;

	/*
	* Checks if the inventory contains an entry for the AssetId
	* Return: True, if an entry is found. False, if no entry is found.
	*/
	bool Contains(const EInventoryType InventoryId, const FPrimaryAssetId EntryId) const;

	/*
	* Checks if the inventory contains an entry for the StackId.
	* Return: True, if an entry is found/ False, if no entry is found.
	*/
	bool Contains(const FInventoryStackId& StackId) const;

	/*
	* Event fired when the inventory contents are changed.
	* Return: The event fired when a change takes place.
	*/
	FInventoryChanged& OnInventoryChanged() { return InventoryChangedEvent; }

	/*
	* Retreives the ID's of all sub inventories owned by this Inventory.
	* Return: Array of EInventoryType with the ID of each owned inventory.
	*/
	const TArray<EInventoryType>& GetSubInventoryIds() const { return ToCreateSubInventories; }

protected:
	UPROPERTY(EditDefaultsOnly)
		int32 NumberOfSlots = 20;

	UPROPERTY(EditDefaultsOnly)
		TArray<EInventoryType> ToCreateSubInventories;

private:
	TMap<EInventoryType, FSubInventory> SubInventories;
	FInventoryChanged InventoryChangedEvent;
};
