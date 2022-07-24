// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TreasureManagerSubsystem.generated.h"

class ACompass;
class ATreasureChest;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UTreasureManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool SpawnLinkedTreasureChest(TSubclassOf<ATreasureChest> TreasureChestClass, TSoftObjectPtr<ACompass> Compass);

protected:
	bool SpawnTreasureChest(TSubclassOf<ATreasureChest> TreasureChestClass);
	bool LinkTreasureToCompass(const TObjectPtr<ATreasureChest> TreasureChest, const TSoftObjectPtr<ACompass> Compass);

	TObjectPtr<ATreasureChest> SpawnedTreasureChest;
};
