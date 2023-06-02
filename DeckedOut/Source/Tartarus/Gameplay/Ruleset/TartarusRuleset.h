// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "TartarusRuleset.generated.h"

class UTartarusRulesetData;
class UTartarusRule;

struct FRulesetStage
{
public:
	FRulesetStage(UObject* Outer, const int32 StageActivationCondition, TArray<TSoftClassPtr<UTartarusRule>>& RuleTemplates);

	int32 GetActivationCondition() const { return ActivationCondition; }
	TArray<UTartarusRule*>& GetRules() { return Rules; }

private:
	FRulesetStage() {};

	int32 ActivationCondition = -1;
	TArray<UTartarusRule*> Rules;
};

/**
 *
 */
UCLASS(Blueprintable)
class TARTARUS_API UTartarusRuleset : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UTartarusRulesetData* const RuleSetData);
	void Activate(UWorld* const InWorld);
	void Deactivate();

protected:
	/*
	* Tries to advance the current stage to the next stage based on the condition.
	* Return: True if advanced to a new stage, false if no advancement is performed.
	*/
	void AdvanceStage(const int32 IntegerConditionValue);

	/*
	* Checks if the condition is satisfied for the provided stage.
	* Return: True if the condition is satisfied, false otherwise.
	*/
	bool ValidateCondition(const int32 CurrentValue, const FRulesetStage& Stage);

	void ApplyStageRules(FRulesetStage& Stage);

	void IntializeStages(UTartarusRulesetData* const RuleSetData);

private:
	UWorld* World = nullptr;
	TArray<FRulesetStage> Stages;
	int32 ActiveStageIndex = INDEX_NONE;
};