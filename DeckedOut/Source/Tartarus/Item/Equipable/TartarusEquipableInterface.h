// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TartarusEquipableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTartarusEquipableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TARTARUS_API ITartarusEquipableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Called when the object is equipped.
	virtual void OnEquipped() = 0;

	// Calleed when the object is unequipped.
	virtual void OnUnequipped() = 0;
};
