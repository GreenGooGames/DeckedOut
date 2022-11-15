// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable/TartarusEquipableInterface.h"
#include "Item/Pickup/TartarusPickup.h"

#include "TartarusEquipableItem.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusEquipableItem : public ATartarusPickup, public ITartarusEquipableInterface
{
	GENERATED_BODY()

#pragma region EquipableInterface
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
#pragma endregion
};
