// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GleipnirItemSubsystem.generated.h"

class UGleipnirItemAsset;

/**
 * 
 */
UCLASS()
class GLEIPNIR_API UGleipnirItemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	TArray<AActor*> SpawnItems(const TArray<TSoftObjectPtr<UGleipnirItemAsset>>& Items, const FTransform& SpawnTransform) const;

private:
	AActor* SpawnItem(const TSubclassOf<AActor>& ItemClass, const FTransform& SpawnTransform) const;

};
