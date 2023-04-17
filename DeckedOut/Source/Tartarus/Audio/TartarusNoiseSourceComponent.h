// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Audio/TartarusSoundData.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TartarusNoiseSourceComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusNoiseSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTartarusNoiseSourceComponent();

public:
	void GenerateNoise(const FTartarusSound& SoundToPlay, const FVector& SourceLocation);

};
