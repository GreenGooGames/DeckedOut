// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleset.h"

bool UTartarusRuleset::ActivateStage(UWorld* const World, int32& ActiveStage, const int32 IntegerConditionValue)
{
	// Activate the last stage that satisfies the condition.
	int32 StageIndex = INDEX_NONE;
	for (int32 i = 0; i < Stages.Num(); i++)
	{
		if (!Stages[i].SatisfiesCondition(IntegerConditionValue))
		{
			break;
		}

		StageIndex = i;
	}

	// Do not activate a stage if no stage satisfies the condition or the stage is already active.
	if (StageIndex == INDEX_NONE || StageIndex == ActiveStage)
	{
		return false;
	}

	ActiveStage = StageIndex;
	Stages[ActiveStage].Activate(World);

	return true;
}

void UTartarusRuleset::Reset(UWorld* const World)
{
	for(FRulesetStage& Stage : Stages)
	{
		Stage.Deactivate(World);
	}
}
