// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TartarusSoundData.generated.h"

UENUM(BlueprintType)
enum class ENoiseLevel : uint8
{
	None,		// No sound
	Faint,		// ex: Crouching, 
	Subtle,		//ex: Walking, Jumping, Opening chest
	Loud,		//ex: Running, Landing from great height, Dropping
	Deafening,	//ex: Bell
};

USTRUCT(BlueprintType)
struct FTartarusSound
{
	GENERATED_BODY()

public:
	USoundBase* GetSound() const { return Sound; }
	ENoiseLevel GetNoiseLevel() const { return NoiseLevel; }

protected:
	// [Koen Goossens] TODO: Load/Play Async
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<USoundBase> Sound;
	
	UPROPERTY(EditDefaultsOnly)
		ENoiseLevel NoiseLevel = ENoiseLevel::None;
};
