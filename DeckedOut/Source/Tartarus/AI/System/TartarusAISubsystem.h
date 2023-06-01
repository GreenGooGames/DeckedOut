// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"

#include "TartarusAISubsystem.generated.h"

class ATartarusAICharacter;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusAISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	FGuid SpawnAI(const FGameplayTag& SpawnPointId, const TMap<TSoftClassPtr<ATartarusAICharacter>, int>& ClassesToSpawnCount);
	void DespawnAI(const FGuid& BatchId);

	void RegisterSpawnLocation(const FGameplayTag& SpawnPointId, const FTransform& SpawnTransform);

protected:
	const FTransform& GetRandomSpawnPoint(const FGameplayTag& SpawnPointId) const;

private:
	TMap<FGameplayTag, TArray<FTransform>> SpawnPoints;
	TMap<FGuid, TArray<ATartarusAICharacter*>> SpawnedAI;
};
