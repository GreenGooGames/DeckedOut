// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "System/TartarusHelpers.h"

#include "TartarusInventoryComponent.generated.h"

UENUM()
enum class EInventoryChanged : uint8
{
	Stored,
	Retrieved
};

USTRUCT()
struct FInventoryItemStack
{
	GENERATED_BODY()

public:
	FInventoryItemStack() {}
	FInventoryItemStack(const int32 NewItemId, const int32 NewStackSize);

	bool operator==(const FInventoryItemStack& rhs) { return GetStackId() == rhs.GetStackId(); }

	int32 StackSize = 0;
	
	const FGuid& GetStackId() const { return StackId; }
	int32 GetItemId() const { return ItemId; }

private:
	FGuid StackId = FGuid();
	int32 ItemId = FTartarusHelpers::InvalidItemId;
};

DECLARE_EVENT_ThreeParams(UTartarusInventoryComponent, FInventoryChanged, EInventoryChanged /*ChangeType*/, FGuid /*StackId*/, int32 /*StackSize*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTartarusInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/*
	* Stores an item into the inventory.
	* Return: The StackId of the stack the item was added to.
	*/
	FGuid StoreItem(const int32 ItemId, const int32 StackSize);

	/*
	* Retrieves an item from the inventory from a random matching stack.
	* Return: True if the item was retrieved, False if the item was not in the inventory or if more than the stored num of items was requested.
	*/
	bool RetrieveItem(const int32 ItemId, const int32 StackSize);

	/*
	* Retrieves an item from the inventory from a specified stack.
	* Return: True if the item was retrieved, False if the item was not in the inventory or if more than the stored num of items was requested.
	*/
	bool RetrieveItem(const FGuid& StackId, const int32 StackSize);

	/*
	* Retrieves an overview of all the slots.
	* Return: Array representing the inventory slots.
	*/
	const TArray<FInventoryItemStack>& GetOverview() const { return InventorySlots; }

	/*
	* Retrieves an overview of all items matching the id.
	* Return: Data stored in the inventory of each slot that contains the given id.
	*/
	const TArray<const FInventoryItemStack*> GetOverviewMulti(const int32 ItemId) const;

	/*
	* Retrieves an overview of a single slot.
	* Return: Data stored in the slot.
	*/
	const FInventoryItemStack* GetOverviewSingle(const FGuid StackId) const;

	/*
	* Retrieves the number of slots in the inventory that are available for storage.
	* Return: Number of avaialble slots.
	*/
	int32 GetAvailableSlotCount() const;

	/*
	* Checks if the inventory contains an entry for the ItemId
	* Return: TRue, if an entry is found/ False, if no entry is found.
	*/
	bool Contains(const int32 ItemId) const;

	/*
	* Checks if the inventory contains an entry for the StackId.
	* Return: TRue, if an entry is found/ False, if no entry is found.
	*/
	bool Contains(const FGuid StackId) const;

	/*
	* Event fired when the inventory contents are changed.
	* Return: The event fired when a change takes place.
	*/
	FInventoryChanged& OnInventoryChanged() { return InventoryChangedEvent; }

protected:
	UPROPERTY(EditDefaultsOnly)
		int32 NumberOfSlots = 20;

	/*
	* Find a slot in the inventory that contains a specified item.
	* Return: The index of the item its slot.
	*/
	int32 FindSlot(const int32 ItemId) const;

	/*
	* Find a slot  in the inventory that corresponds with the StackId.
	* Return: The index of the item its slot.
	*/
	int32 FindSlot(const FGuid& StackId) const;

	/*
	* Reduces the StackSize of the given slot by the given stackSize.
	* Return: True if the stacksize is reduced. False if the stacksize is not reduced.
	*/
	bool ReduceStack(const int32 SlotIndex, const int32 StackSize);

private:
	TArray<FInventoryItemStack> InventorySlots;
	FInventoryChanged InventoryChangedEvent;
};