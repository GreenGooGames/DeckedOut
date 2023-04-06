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
}

bool ATartarusTreasureHuntGameState::IsTreasureHuntActive() const
{
	return TreasureHuntState == ETreasureHuntState::Active;
}
#pragma endregion

void ATartarusTreasureHuntGameState::EditGameModifier(const EGameModifier Modifier, const float Adjustment)
{
	switch (Modifier)
	{
	case EGameModifier::Sneak:
	{
		GameModifiers.SneakModifier += Adjustment;
		break;
	}
	case EGameModifier::Silence:
	{
		GameModifiers.SilenceModifier += Adjustment;
		break;
	}
	// Intentional fall-trough
	case EGameModifier::None:
	default:
		break;
	}
}
