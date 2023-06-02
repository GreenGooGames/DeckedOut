// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusRulesetData.generated.h"

class UTartarusRule;

UCLASS()
class TARTARUS_API UTartarusRulesetStageData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Integer value to reach to activate the rules of this stage.
	UPROPERTY(EditDefaultsOnly)
		int32 IntegerCondition = 0;

	// The rules to apply during this stage.
	UPROPERTY(EditDefaultsOnly)
		TArray<TSoftClassPtr<UTartarusRule>> RuleTemplates;
};

UCLASS()
class TARTARUS_API UTartarusRulesetData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Collection of all the stages data.
	UPROPERTY(EditDefaultsOnly)
		TArray<TObjectPtr<UTartarusRulesetStageData>> StageTemplates;
};
