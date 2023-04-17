// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TartarusInteractableTargetInterface.generated.h"

class AController;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTartarusInteractableTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TARTARUS_API ITartarusInteractableTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	Checks if anything is blocking interaction.
	Return: True, Interaction is possible. False, Interaction is being blocked.
	*/
	virtual bool IsInteractable() const = 0;

	/*
	Tries to start an interaction.
	Return: True, Interaction has started. False, Interaction failed to start.
	*/
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) = 0;

};
