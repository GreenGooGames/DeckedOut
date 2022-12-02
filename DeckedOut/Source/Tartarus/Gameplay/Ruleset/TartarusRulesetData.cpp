// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRulesetData.h"

#include "Gameplay/Ruleset/TartarusRule.h"

bool FRulesetStage::SatisfiesCondition(const int32 IntegerConditionValue)
{
	if (IntegerConditionValue < IntegerCondition)
	{
		return false;
	}

	return true;
}

void FRulesetStage::Activate(UWorld* const World)
{
	for (TObjectPtr<UTartarusRule> Rule : Rules)
	{
		if (!IsValid(Rule))
		{
			return;
		}

		Rule->StartRule(World);
	}
}

void FRulesetStage::Deactivate(UWorld* const World)
{
	for (TObjectPtr<UTartarusRule> Rule : Rules)
	{
		if (!IsValid(Rule))
		{
			return;
		}

		Rule->StopRule(World);
	}
}
