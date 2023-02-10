// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Inventory/TartarusInventoryData.h"

#include "TartarusEquipableData.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EEquipmentSlot : uint8
{
	None = 0x00,
	LeftHand = 0x01,
	RightHand = 0x02
};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
	GENERATED_BODY()

public:
	/*
	* Retrieves the item that is being held by this slot.
	* Return: Pointer to the item being held, nullptr if no item exist.
	*/
	ATartarusItemBase* GetItem() const { return Item.Get(); }

	// Sets the Item instance that is equipped to this slot.
	void SetItem(ATartarusItemBase* const ItemInstance) { Item = ItemInstance; }

	/*
	* Retrieves the InventoryStackId that is used to reference the equipped item in the owner inventory.
	* Return: Id of the ItemStack that represents the equipped item in the owner inventory.
	*/
	const FInventoryStackId& GetInventoryStackId() const { return InventoryStackId; }

	void SetInventoryItemStackId(const FInventoryStackId& InventoryStackIdReference);

	/*
	* Retrieves the name of the socket that is linked to this slot.
	* Return: Name of the socket to attach to.
	*/
	const FName& GetSocket() { return Socket; }

	// Resets all data stored.
	void Reset();

protected:
	// Optional Socket name to attach to.
	UPROPERTY(EditDefaultsOnly)
		FName Socket = NAME_None;

private:
	TWeakObjectPtr<ATartarusItemBase> Item = nullptr;
	FInventoryStackId InventoryStackId = FInventoryStackId();
};
