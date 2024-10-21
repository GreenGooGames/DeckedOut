// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/System/TartarusAISubsystem.h"

#include "Logging/TartarusLogChannels.h"
#include "Engine/World.h"
#include "AI/TartarusAICharacter.h"

FGuid UTartarusAISubsystem::SpawnAI(const FGameplayTag& SpawnPointId, const TMap<TSoftClassPtr<ATartarusAICharacter>, int>& ClassesToSpawnCount)
{
	if (ClassesToSpawnCount.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn AI: No AI to spawn!"), *FString(__FUNCTION__));
		return FGuid();
	}

	const FGuid BatchId = FGuid::NewGuid();
	const FTransform SpawnTransformCenter = GetRandomSpawnPoint(SpawnPointId);
	for (auto& ClassToSpawnCount : ClassesToSpawnCount)
	{
		// TODO: Pick a Location around the SpawnTrasnformCenter to spawn the AI at based on how many need to spawn.
		// TODO: ASync loading
		ATartarusAICharacter* const SpawnedCharacter = GetWorld()->SpawnActor<ATartarusAICharacter>(ClassToSpawnCount.Key.LoadSynchronous(), SpawnTransformCenter);
		if (!IsValid(SpawnedCharacter))
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn AI: Could not spawn the AI!"), *FString(__FUNCTION__));
			continue;
		}

		SpawnedAI.FindOrAdd(BatchId).Add(SpawnedCharacter);
	}

	return BatchId;
}

void UTartarusAISubsystem::DespawnAI(const FGuid& BatchId)
{
	TArray<ATartarusAICharacter*> BatchAI = SpawnedAI.FindRef(BatchId);
	for (ATartarusAICharacter* const Character : BatchAI)
	{
		if (!IsValid(Character))
		{
			continue;
		}

		Character->Destroy();
	}

	SpawnedAI.Remove(BatchId);
}

void UTartarusAISubsystem::RegisterSpawnLocation(const FGameplayTag& SpawnPointId, const FTransform& SpawnTransform)
{
	if (!SpawnPointId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to register spawnpoint: SpawnPoint ID is invalid!"), *FString(__FUNCTION__));
		return;
	}

	SpawnPoints.FindOrAdd(SpawnPointId).Add(SpawnTransform);
}

const FTransform UTartarusAISubsystem::GetRandomSpawnPoint(const FGameplayTag& SpawnPointId) const
{
	TArray<FTransform> Spawns = SpawnPoints.FindRef(SpawnPointId);
	if (Spawns.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve a valid spawnpoint: No spawnpoints registered for the given id!"), *FString(__FUNCTION__));
		return FTransform();
	}

	return Spawns[FMath::RandRange(0, Spawns.Num() - 1)];
}
