// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Hazard/TartarusHazardData.h"
#include "Gameplay/Ruleset/TartarusRule.h"

#include "TartarusRuleHazard.generated.h"

/**
 * Activates all hazards in the world of the specified HazardType.
 */
UCLASS(Blueprintable)
class TARTARUS_API UTartarusRuleHazard : public UTartarusRule
{
	GENERATED_BODY()
	
public:
	virtual bool StartRule(UWorld* const World) override;
	virtual bool StopRule(UWorld* const World) override;

protected:
	// The type of hazard affected by this rule.
	UPROPERTY(EditDefaultsOnly)
		EHazardType HazardType = EHazardType::None;
};
