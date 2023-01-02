// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TartarusHazardSubsystem.generated.h"

class ATartarusHazard;

enum class EHazardType : uint8;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusHazardSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	// Registers a hazard to the subsystem.
	void RegisterHazard(ATartarusHazard* const Hazard);

	// Activates all hazards of the given type.
	void ActivateHazards(const EHazardType HazardType) const;

	// Deactivates all hazards of the given type.
	void DeactivateHazards(const EHazardType HazardType) const;

	// Deactives all hazards managed by this subsystem.
	void DeactivateAllHazards() const;

private:
	// Map containing all hazards sorted per type.
	TMap<EHazardType, TArray<TObjectPtr<ATartarusHazard>>> Hazards;
};
