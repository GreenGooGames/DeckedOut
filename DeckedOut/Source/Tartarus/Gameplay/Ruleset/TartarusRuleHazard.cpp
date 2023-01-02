// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleHazard.h"

#include "Gameplay/Hazard/TartarusHazardSubsystem.h"

bool UTartarusRuleHazard::StartRule(UWorld* const World)
{
	if (!World)
	{
		return false;
	}

	UTartarusHazardSubsystem* const HazardSubsystem = World->GetSubsystem<UTartarusHazardSubsystem>();
	if (!IsValid(HazardSubsystem))
	{
		return false;
	}

	HazardSubsystem->ActivateHazards(HazardType);

	return true;
}

bool UTartarusRuleHazard::StopRule(UWorld* const World)
{
	if (!World)
	{
		return false;
	}

	UTartarusHazardSubsystem* const HazardSubsystem = World->GetSubsystem<UTartarusHazardSubsystem>();
	if (!IsValid(HazardSubsystem))
	{
		return false;
	}

	HazardSubsystem->DeactivateHazards(HazardType);

	return true;
}
