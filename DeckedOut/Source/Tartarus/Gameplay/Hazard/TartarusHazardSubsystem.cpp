// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Hazard/TartarusHazardSubsystem.h"

#include "Gameplay/Hazard/TartarusHazard.h"
#include "Gameplay/Hazard/TartarusHazardData.h"

void UTartarusHazardSubsystem::RegisterHazard(ATartarusHazard* const Hazard)
{
	if (!IsValid(Hazard))
	{
		return;
	}

	const EHazardType Type = Hazard->GetType();
	if (!Hazards.Contains(Type))
	{
		Hazards.Add(Type);
	}
	
	Hazards[Type].Add(Hazard);
}

void UTartarusHazardSubsystem::ActivateHazards(const EHazardType HazardType) const
{
	if (!Hazards.Contains(HazardType))
	{
		return;
	}

	for (ATartarusHazard* const Hazard : Hazards[HazardType])
	{
		if (!IsValid(Hazard))
		{
			continue;
		}

		Hazard->Activate();
	}
}

void UTartarusHazardSubsystem::DeactivateHazards(const EHazardType HazardType) const
{
	if (!Hazards.Contains(HazardType))
	{
		return;
	}

	for (ATartarusHazard* const Hazard : Hazards[HazardType])
	{
		if (!IsValid(Hazard))
		{
			continue;
		}

		Hazard->Deactivate();
	}
}

void UTartarusHazardSubsystem::DeactivateAllHazards() const
{
	for (auto HazardsOfType : Hazards)
	{
		for (ATartarusHazard* const Hazard : HazardsOfType.Value)
		{
			if (!IsValid(Hazard))
			{
				continue;
			}

			Hazard->Deactivate();
		}
	}
}