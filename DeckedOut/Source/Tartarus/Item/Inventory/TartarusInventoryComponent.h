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
	* Retrieves an overview of all the slots.
	* Return: Array representing the inventory slots.
	*/
	const TArray<FInventoryItemStack>& GetOverview() const { return InventorySlots; }

	/*
	* Retrieves all data of a stack.
	* Return: true if the stack was found, false if not found.
	*/
	bool FindStack(const FGuid& StackId, FInventoryItemStack& OutStack);

	/*
	* Event fired when the inventory contents are changed.
	* Return: The event fired when a change takes place.
	*/
	FInventoryChanged& OnInventoryChanged() { return InventoryChangedEvent; }

protected:
	UPROPERTY(EditDefaultsOnly)
		int32 NumberOfSlots = 20;

	/*
	* Find an item in the inventory.
	* Return: The index of the item its slot.
	*/
	int32 FindItem(const int32 ItemId) const;

private:
	TArray<FInventoryItemStack> InventorySlots;
	FInventoryChanged InventoryChangedEvent;
};
