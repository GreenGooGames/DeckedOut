// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"

#pragma region TreasureHuntState
void ATartarusTreasureHuntGameState::ChangeTreasureHuntState(const ETreasureHuntState NewState)
{
	ETreasureHuntState OldState = TreasureHuntState;

	if (NewState == OldState)
	{
		return;
	}

	TreasureHuntState = NewState;

	// Notify all watchers that the state has changed to a new state.
	OnRunningStateChanged().Broadcast(OldState, TreasureHuntState);

	// Handle behavior for when a game has ended.
	if (TreasureHuntState == ETreasureHuntState::Inactive)
	{
		// [Koen Goossens] TODO: To streamline, create a method HandleTreasureHuntEnd and sub it to the above delegate.
		ResetClank();
	}
}

bool ATartarusTreasureHuntGameState::IsTreasureHuntActive() const
{
	return TreasureHuntState == ETreasureHuntState::Active;
}
#pragma endregion

#pragma region Clank
void ATartarusTreasureHuntGameState::IncreaseClank(const int32 GeneratedClank)
{
	ClankLevel += GeneratedClank;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("IncreasedClank: ") + FString::FromInt(ClankLevel));
#endif
}

void ATartarusTreasureHuntGameState::ResetClank()
{
	ClankLevel = 0;
}
#pragma endregion
