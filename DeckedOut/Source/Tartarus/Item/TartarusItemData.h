// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "Item/TartarusItemBase.h"
#include "Item/Equipable/TartarusEquipableData.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "System/TartarusHelpers.h"

#include "TartarusItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Artifact = 1,
	TreasureKey = 2,
	// Bitflags?
	// Enum = 4
	// Enum = 8
	// enum = 16
	// Enum = 32
	// Enum = 64
	// Enum = 128
};

// General information of an item.
USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Unique Id to identify the item.
	// Negative = non-stackable Item, Positive = Stackable item, 0 = invalid
	UPROPERTY(EditDefaultsOnly)
		int32 UniqueItemId = FTartarusHelpers::InvalidItemId;

	// The type of the item.
	UPROPERTY(EditDefaultsOnly)
		EItemType ItemType = EItemType::None;

	// The id of the inventory to store the item in.
	UPROPERTY(EditDefaultsOnly)
		EInventoryType InventoryType = EInventoryType::MAX;

	// Name to represent the item.
	UPROPERTY(EditDefaultsOnly)
		FText Name = FText();
	
	// Description to provide information about the item.
	UPROPERTY(EditDefaultsOnly)
		FText Description = FText();

	// Reference blueprint to instaniate when spawned in the world.
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<ATartarusItemBase> Blueprint = nullptr;

	// Reference texture to represent the item in UI.
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UTexture2D> DisplayTexture = nullptr;

	// Should this item be auto-equipped if possible when received?
	UPROPERTY(EditDefaultsOnly)
		bool bCanAutoEquip = false;

	// To which slots can this be equipped to.
	UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = "EEquipmentSlot"))
		uint8 EquipableSlots = 0;
};
