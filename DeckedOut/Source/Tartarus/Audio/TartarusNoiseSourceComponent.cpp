// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/TartarusNoiseSourceComponent.h"

#include "Audio/TartarusSoundData.h"
#include "Gameplay/Clank/TartarusClankSubsystem.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTartarusNoiseSourceComponent::UTartarusNoiseSourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTartarusNoiseSourceComponent::GenerateNoise(const FTartarusSound& SoundToPlay, const FVector& SourceLocation)
{
	// Play the sound in the world on the location.
	UGameplayStatics::PlaySoundAtLocation(GetOwner(), SoundToPlay.GetSound(), SourceLocation);

	// Convert the noise thats been made to clank.
	UTartarusClankSubsystem* const ClankSubsystem = GetWorld()->GetSubsystem<UTartarusClankSubsystem>();
	if (!IsValid(ClankSubsystem))
	{
		return;
	}

	ClankSubsystem->GenerateClank(SoundToPlay.GetNoiseLevel());
}
