// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Clank/TartarusClankSubsystem.h"

#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"

void UTartarusClankSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	ATartarusTreasureHuntGameState* const GameState = InWorld.GetGameState<ATartarusTreasureHuntGameState>();
	
	if (!IsValid(GameState))
	{
		return;
	}

	GameState->OnRunningStateChanged().AddUObject(this, &UTartarusClankSubsystem::HandleGameRunningStateChanged);
}

void UTartarusClankSubsystem::GenerateClank(const ENoiseLevel NoiseLevel)
{
	const ENoiseLevel Volume = ApplyGameModifers(NoiseLevel);
	if (Volume == ENoiseLevel::None)
	{
		return;
	}

	ClankLevel += BaseClankGenerationLevel * (uint8)Volume;

	ClankLevelChangedEvent.Broadcast(ClankLevel);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("IncreasedClank: ") + FString::FromInt(ClankLevel));
#endif
}

void UTartarusClankSubsystem::ResetClank()
{
	ClankLevel = 0;
}

void UTartarusClankSubsystem::HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState)
{
	if (OldState == NewState)
	{
		return;
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	switch (NewState)
	{
	case ETreasureHuntState::Active:
	{
		// Start passive clank generation.
		if (TimerManager.IsTimerPaused(PassiveClankGenerationTimerHandle))
		{
			TimerManager.UnPauseTimer(PassiveClankGenerationTimerHandle);
		}
		else
		{
			TimerManager.SetTimer(PassiveClankGenerationTimerHandle, this, &UTartarusClankSubsystem::GeneratePassiveClank, ClankGenerationInterval, true);
		}
		break;
	}
	case ETreasureHuntState::Paused:
	{
		// Pause passive clank generation.
		TimerManager.PauseTimer(PassiveClankGenerationTimerHandle);
		break;
	}
	case ETreasureHuntState::Inactive:
	{
		// Stop passive Clank generation;
		TimerManager.ClearTimer(PassiveClankGenerationTimerHandle);
		ResetClank();
		break;
	}
	default:
		break;
	}
}

void UTartarusClankSubsystem::GeneratePassiveClank()
{
	GenerateClank(PassiveClankVolume);
}

ENoiseLevel UTartarusClankSubsystem::ApplyGameModifers(const ENoiseLevel NoiseLevel) const
{
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		return NoiseLevel;
	}

	const FGameModifiers& Modifiers = GameState->GetGameModifiers();
	ENoiseLevel Volume = NoiseLevel;
	const float Random = FMath::FRand();

	// Check to completely silence the sound.
	if (Modifiers.SilenceModifier > Random)
	{
		Volume = ENoiseLevel::None;

#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Silenced volume when generating clank!"));
#endif
	}
	// Check to reduce the volume.
	else if (Modifiers.SneakModifier > Random)
	{
		Volume = static_cast<ENoiseLevel>((uint8)Volume - 1);

#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Lowered volume when generating clank: ") + FString::FromInt((uint8)Volume));
#endif
	}

	return Volume;
}