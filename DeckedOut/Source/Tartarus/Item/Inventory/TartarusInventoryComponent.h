// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "System/TartarusHelpers.h"

#include "TartarusInventoryComponent.generated.h"

USTRUCT()
struct FInventoryItemStack
{
	GENERATED_BODY()

public:
	int32 ItemId = FTartarusHelpers::InvalidItemId;
	
	int32 StackSize = 0;
};

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
	* Return: True if the item is stored, False if the item could not be stored.
	*/
	bool StoreItem(const int32 ItemId, const int32 StackSize);

	/*
	* Retrieves an item from the inventory.
	* Return: True if the item was retrieved, False if the item was not in the inventory or if more than the stored num of items was requested.
	*/
	bool RetrieveItem(const int32 ItemId, const int32 StackSize);

	/*
	* Retrieves an overview of all the slots.
	* Return: Array representing the inventory slots.
	*/
	const TArray<FInventoryItemStack>& GetOverview() const { return InventorySlots; }

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
};
