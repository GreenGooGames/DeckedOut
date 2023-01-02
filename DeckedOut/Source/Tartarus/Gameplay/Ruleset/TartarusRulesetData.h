// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TartarusRulesetData.generated.h"

class UTartarusRule;

USTRUCT(BlueprintType)
struct FRulesetStage
{
	GENERATED_BODY()

public:
	bool SatisfiesCondition(const int32 IntegerConditionValue);
	void Activate(UWorld* const World);
	void Deactivate(UWorld* const World);

protected:
	// Integer value to reach to activate the rules of this stage.
	UPROPERTY(EditDefaultsOnly)
		int32 IntegerCondition = 0;

	// [Koen Goossens] TODO: ASync SoftClassPtr.
	// The rules to apply during this stage.
	UPROPERTY(EditDefaultsOnly)
		TArray<TObjectPtr<UTartarusRule>> Rules;
};