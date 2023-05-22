// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/TartarusInventoryData.h"
#include "Item/TartarusItemData.h"
#include "TartarusItemInstance.h"
#include "GameplayTagContainer.h"

#include "TartarusItem.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
#pragma region General_Info
	// Name to represent the item.
	UPROPERTY(EditDefaultsOnly, Category = "General")
		FText Name = FText();

	// Description to provide information about the item.
	UPROPERTY(EditDefaultsOnly, Category = "General")
		FText Description = FText();

	// Reference blueprint to instaniate when spawned in the world.
	UPROPERTY(EditDefaultsOnly, Category = "General")
		TSoftClassPtr<ATartarusItemInstance> Blueprint = nullptr;

	// Reference texture to represent the item in UI.
	UPROPERTY(EditDefaultsOnly, Category = "General")
		TSoftObjectPtr<UTexture2D> DisplayTexture = nullptr;

	// The type of the item.
	UPROPERTY(EditDefaultsOnly, Category = "General")
		EItemType ItemType = EItemType::None;
#pragma endregion

#pragma region Inventory_Info
	// The id of the inventory to store the item in.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		FGameplayTag InventoryId = FGameplayTag::EmptyTag;

	// Is the item stackable in the inventory?
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		bool bIsStackable = false;

#pragma endregion

#pragma region Equip_Info
	// To which slots can this be equipped to.
	UPROPERTY(EditDefaultsOnly, Category = "Equip", meta = (Bitmask, BitmaskEnum = "EEquipmentSlot"))
		uint8 EquipableSlots = 0;

	// Should this item be auto-equipped if possible when received?
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
		bool bCanAutoEquip = false;
#pragma endregion
};
