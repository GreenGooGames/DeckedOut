// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Hazard/TartarusHazardData.h"

#include "TartarusHazard.generated.h"

UCLASS()
class TARTARUS_API ATartarusHazard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusHazard();

protected:
	virtual void BeginPlay() override;

public:
	// Activates this hazard its logic.
	virtual void Activate();

	// Deactivates this hazard its logic.
	virtual void Deactivate();

	/*
	* Retrieves the kind of type this hazard is.
	* Return: the type of this hazard as a EHazardType.
	*/
	EHazardType GetType() { return HazardType; }

protected:
	// The kind of type this hazard is.
	UPROPERTY(EditDefaultsOnly)
		EHazardType HazardType = EHazardType::None;

	// BP behavior to happen when the hazard activates.
	UFUNCTION(BlueprintImplementableEvent)
		void BP_HandleActivated();

	// BP behavior to happen when the hazard activates.
	UFUNCTION(BlueprintImplementableEvent)
		void BP_HandleDeactivated();
};
