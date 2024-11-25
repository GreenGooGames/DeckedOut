// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GleipnirLootComponent.generated.h"

class UGleipnirItemAsset;
class UGleipnirLootTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLEIPNIR_API UGleipnirLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGleipnirLootComponent();

	bool DropLoot(const FTransform& Transform) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UGleipnirLootTable> LootTable = nullptr;

private:
	TArray<TSoftObjectPtr<UGleipnirItemAsset>> GetLoot() const;
};
