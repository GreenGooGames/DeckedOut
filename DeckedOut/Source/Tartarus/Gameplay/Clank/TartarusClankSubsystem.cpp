// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Clank/TartarusClankSubsystem.h"

#include "Audio/TartarusSoundData.h"
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
	if (NoiseLevel == ENoiseLevel::None)
	{
		return;
	}

	ClankLevel += BaseClankGenerationLevel * (uint8)NoiseLevel;

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
	GenerateClank(ENoiseLevel::Faint);
}
