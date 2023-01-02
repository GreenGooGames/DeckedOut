// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Ruleset/TartarusRule.h"

#include "TartarusRuleSpawnAI.generated.h"

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
	UPROPERTY(EditDefaultsOnly)
		int32 NumToSpawn = 0;

	// UPROPERTY(EditDefaultsOnly)
	//	TArray<TSoftClassPtr<TartarusAICharacter>> AIClasses;

private:
	// FGuid AIBatchId = FGuid();
};
