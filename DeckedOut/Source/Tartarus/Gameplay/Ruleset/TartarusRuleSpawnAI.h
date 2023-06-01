// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Ruleset/TartarusRule.h"
#include "GameplayTagContainer.h"

#include "TartarusRuleSpawnAI.generated.h"

class ATartarusAICharacter;

/**
 * 
 */
UCLASS(Blueprintable)
class TARTARUS_API UTartarusRuleSpawnAI : public UTartarusRule
{
	GENERATED_BODY()
	
public:
	virtual bool StartRule(UWorld* const World) override;
	virtual bool StopRule(UWorld* const World) override;

protected:
	// List of the AI to spawn and how many.
	 UPROPERTY(EditDefaultsOnly)
		TMap<TSoftClassPtr<ATartarusAICharacter>, int> ClassesToSpawnCount;

	 // The ID of the spawnpoints that are a valid target to spawn on.
	 UPROPERTY(EditDefaultsOnly)
		 FGameplayTag SpawnPointId = FGameplayTag::EmptyTag;

private:
	FGuid AIBatchId = FGuid();
};
