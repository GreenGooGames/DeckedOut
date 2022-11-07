// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable/TartarusEquipableInterface.h"
#include "Item/TartarusItemBase.h"

#include "TartarusEquipableItem.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusEquipableItem : public ATartarusItemBase, public ITartarusEquipableInterface
{
	GENERATED_BODY()

#pragma region EquipableInterface
public:
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
#pragma endregion

#pragma region TartarusInteractableTargetInterface
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
