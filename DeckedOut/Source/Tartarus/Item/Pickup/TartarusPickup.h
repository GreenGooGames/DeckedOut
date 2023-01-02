// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TartarusItemBase.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusPickup.generated.h"

/**
 * A class for items that can be picked up and stored in the inventory without additional behavior.
 * To add extra behavior override HandlePickedup.
 */
UCLASS()
class TARTARUS_API ATartarusPickup : public ATartarusItemBase, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:
	ATartarusPickup();

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	/*
	* Defines the behavior to execute when this item is pickedup in the world.
	* Return: Has the behavior been handles succesfully or not.
	*/
	virtual bool HandlePickedup(const TObjectPtr<AController> InstigatorController);

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
