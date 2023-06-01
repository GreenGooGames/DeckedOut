// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleSpawnAI.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Logging/TartarusLogChannels.h"
#include "AI/System/TartarusAISubsystem.h"

bool UTartarusRuleSpawnAI::StartRule(UWorld* const World)
{
	if (!IsValid(World))
	{
		return false;
	}

	if (AIBatchId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Spawn AI: There already is a valid batch!"), *FString(__FUNCTION__));
		return false;
	}

	UTartarusAISubsystem* const AISubsystem = World->GetSubsystem<UTartarusAISubsystem>();
	if(!IsValid(AISubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn AI: Could not find the AISubsystem!"), *FString(__FUNCTION__));
		return false;
	}

	AIBatchId = AISubsystem->SpawnAI(SpawnPointId, ClassesToSpawnCount);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Spawned AI: ") + AIBatchId.ToString());
#endif

	return true;
}

bool UTartarusRuleSpawnAI::StopRule(UWorld* const World)
{
	if (!IsValid(World))
	{
		return false;
	}

	UTartarusAISubsystem* const AISubsystem = World->GetSubsystem<UTartarusAISubsystem>();
	if(!IsValid(AISubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Despawn AI: Could not find the AISubsystem!"), *FString(__FUNCTION__));
		return false;
	}
	
	AISubsystem->DespawnAI(AIBatchId);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Despawned AI: ") + AIBatchId.ToString());
#endif

	AIBatchId = FGuid();

	return true;
}
