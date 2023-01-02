// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/TartarusNoiseSourceComponent.h"

#include "Audio/TartarusSoundData.h"
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
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		return;
	}

	const int32 ToGenerateClank = ConvertNoiseToClank(SoundToPlay.GetNoiseLevel());
	GameState->IncreaseClank(ToGenerateClank);
}

int32 UTartarusNoiseSourceComponent::ConvertNoiseToClank(const ENoiseLevel NoiseLevel)
{
	int32 ToClank = 0;

	switch (NoiseLevel)
	{
	case ENoiseLevel::Faint:
		ToClank = 1;
		break;
	case ENoiseLevel::Subtle:
		ToClank = 2;
		break;
	case ENoiseLevel::Loud:
		ToClank = 4;
		break;
	case ENoiseLevel::Deafening:
		ToClank = 8; 
		break;
	case ENoiseLevel::None:
	default:
		break;
	}

	return ToClank;
}
