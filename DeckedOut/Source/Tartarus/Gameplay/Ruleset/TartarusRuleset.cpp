// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleset.h"

void UTartarusRuleset::ActivateStage(UWorld* const World, const int32 IntegerConditionValue)
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

	if (StageIndex == INDEX_NONE)
	{
		return;
	}

	Stages[StageIndex].Activate(World);
}

void UTartarusRuleset::Reset(UWorld* const World)
{
	for(FRulesetStage& Stage : Stages)
	{
		Stage.Deactivate(World);
	}
}
