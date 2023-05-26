// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusGameModifier.generated.h"

UENUM(BlueprintType)
enum class EGameModifier : uint8
{
	None,
	Sneak,
	Silence
};

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameModifier : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FText& GetName() const { return ModifierName; }
	const FText& GetDescription() const { return ModifierDescription; }
	const EGameModifier& GetType() const { return ModifierType; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FText ModifierName = FText();
	UPROPERTY(EditDefaultsOnly)
		FText ModifierDescription = FText();

	UPROPERTY(EditDefaultsOnly)
		EGameModifier ModifierType = EGameModifier::None;
};
