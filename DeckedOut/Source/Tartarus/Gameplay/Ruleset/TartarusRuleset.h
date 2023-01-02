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
	/*
	* Checks all stages and if any satisfies the condition activate it.
	* Return: True if the the stage has changed, false if no stage has been activated.
	* Return: ActiveStage, updated to reflect the new active stage.
	*/
	bool ActivateStage(UWorld* const World, int32& ActiveStage, const int32 IntegerConditionValue);

	// Deactivates all stages setup for this ruleset.
	void Reset(UWorld* const World);

protected:
	// Defines which rules will be applied during each stage.
	UPROPERTY(EditDefaultsOnly)
		TArray<FRulesetStage> Stages;
};