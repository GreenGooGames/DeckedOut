// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleset.h"

#include "Gameplay/Clank/TartarusClankSubsystem.h"
#include "Gameplay/Ruleset/TartarusRulesetData.h"
#include "Gameplay/Ruleset/TartarusRule.h"
#include "Gameplay/Clank/TartarusClankSubsystem.h"
#include "Engine/World.h"

void UTartarusRuleset::Initialize(UTartarusRulesetData* const RuleSetData)
{
	IntializeStages(RuleSetData);
}

void UTartarusRuleset::Activate(UWorld* const InWorld)
{
	World = InWorld;

	AdvanceStage(0);

	// TODO: Sub to the Clank Subsytem and each time the clank is increased call AdvanceStage.
	UTartarusClankSubsystem* const ClankSubsystem = World->GetSubsystem<UTartarusClankSubsystem>();
	if (IsValid(ClankSubsystem))
	{
		ClankSubsystem->OnClankLevelChanged().AddUObject(this, &UTartarusRuleset::AdvanceStage);
	}
}

void UTartarusRuleset::Deactivate()
{
	for (FRulesetStage& Stage : Stages)
	{
		for (UTartarusRule* const Rule : Stage.GetRules())
		{
			Rule->StopRule(World);
		}
	}

	UTartarusClankSubsystem* const ClankSubsystem = World->GetSubsystem<UTartarusClankSubsystem>();
	if (IsValid(ClankSubsystem))
	{
		ClankSubsystem->OnClankLevelChanged().RemoveAll(this);
	}
}

void UTartarusRuleset::AdvanceStage(const int32 IntegerConditionValue)
{
	// Check what the last stage is that could be activated.
	int32 StageIndex = INDEX_NONE;
	for (int32 i = 0; i < Stages.Num(); i++)
	{
		if (!ValidateCondition(IntegerConditionValue, Stages[i]))
		{
			break;
		}

		StageIndex = i;
	}

	// Do not activate a stage if no stage satisfies the condition or the stage is already active.
	if (StageIndex == INDEX_NONE || StageIndex == ActiveStageIndex)
	{
		return;
	}

	// Apply the rules of each stage from the ActiveStage (excluding) up to and including the NewStage.
	for (int32 i = ActiveStageIndex + 1; i <= StageIndex; i++)
	{
		ApplyStageRules(Stages[i]);
	}

	ActiveStageIndex = StageIndex;

	return;
}

bool UTartarusRuleset::ValidateCondition(const int32 CurrentValue, const FRulesetStage& Stage)
{
	// Fail if the passed value is less then the stage condition.
	if (CurrentValue < Stage.GetActivationCondition())
	{
		return false;
	}
	return true;
}

void UTartarusRuleset::ApplyStageRules(FRulesetStage& Stage)
{
	for (UTartarusRule* const Rule : Stage.GetRules())
	{
		Rule->StartRule(World);
	}
}

void UTartarusRuleset::IntializeStages(UTartarusRulesetData* const RuleSetData)
{
	for (TObjectPtr<UTartarusRulesetStageData>& StageData : RuleSetData->StageTemplates)
	{
		FRulesetStage Stage = FRulesetStage(this, StageData->IntegerCondition, StageData->RuleTemplates);
		Stages.Add(Stage);
	}
}

FRulesetStage::FRulesetStage(UObject* Outer, const int32 StageActivationCondition, TArray<TSoftClassPtr<UTartarusRule>>& RuleTemplates)
{
	ActivationCondition = StageActivationCondition;

	for (TSoftClassPtr<UTartarusRule>& RuleTemplate : RuleTemplates)
	{
		// TODO: Async Loading
		UTartarusRule* const Rule = NewObject<UTartarusRule>(Outer, RuleTemplate.LoadSynchronous());
		if (!IsValid(Rule))
		{
			continue;
		}

		Rules.Add(Rule);
	}
}
