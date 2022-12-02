// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gameplay/Ruleset/TartarusRulesetData.h"

#include "TartarusRuleset.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class TARTARUS_API UTartarusRuleset : public UDataAsset
{
	GENERATED_BODY()

public:
	void ActivateStage(UWorld* const World, const int32 IntegerConditionValue);
	void Reset(UWorld* const World);

protected:
	// Defines which rules will be applied during each stage.
	UPROPERTY(EditDefaultsOnly)
		TArray<FRulesetStage> Stages;
};