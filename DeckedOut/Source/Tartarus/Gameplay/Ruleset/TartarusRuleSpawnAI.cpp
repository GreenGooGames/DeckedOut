// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleSpawnAI.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

bool UTartarusRuleSpawnAI::StartRule(UWorld* const World)
{
	if (!IsValid(World))
	{
		return false;
	}

	// UTartarusAISubsystem* const AISubsystem = World()->GetSubsystem<TartarusAISubsystem>();
	// if(!IsValid(AISubsystem))
	// {
	//		return false;
	// }
	//
	// AIBatchId = AISubsystem->SpawnAI(AIClasses, NumToSpawn);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Spawned AI: ") + FString::FromInt(NumToSpawn));
#endif

	return true;
}

bool UTartarusRuleSpawnAI::StopRule(UWorld* const World)
{
	if (!IsValid(World))
	{
		return false;
	}

	// UTartarusAISubsystem* const AISubsystem = World()->GetSubsystem<TartarusAISubsystem>();
	// if(!IsValid(AISubsystem))
	// {
	//		return false;
	// }
	//
	// AISubsystem->DespawnAI(AIBatchId);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Despawned AI: ") /* + AIBatchId.ToString() */);
#endif

	return true;
}
