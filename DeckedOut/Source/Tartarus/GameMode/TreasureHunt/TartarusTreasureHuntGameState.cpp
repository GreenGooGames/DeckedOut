// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "TartarusTreasureHuntGameState.h"
#include "Gameplay/Ruleset/TartarusGameModifier.h"

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

#pragma region GameModifiers
const TPair<TObjectPtr<const UTartarusGameModifier>, float>* ATartarusTreasureHuntGameState::FindGameModifier(const EGameModifier ModifierType)
{
	for (auto& GameModifier : GameModifiers)
	{
		if (GameModifier.Key->GetType() != ModifierType)
		{
			continue;
		}

		return &GameModifier;
	}

	return nullptr;
}

void ATartarusTreasureHuntGameState::EditGameModifier(const UTartarusGameModifier* const Modifier, const float Adjustment)
{
	if (!IsValid(Modifier))
	{
		return;
	}

	bool bHasModified = false;
	for (auto GameModifier : GameModifiers)
	{
		if (GameModifier.Key->GetType() != Modifier->GetType())
		{
			continue;
		}

		GameModifier.Value += Adjustment;
		bHasModified = true;
	}

	if (!bHasModified)
	{
		GameModifiers.Add(Modifier, Adjustment);
	}

	OnGameModifiersChanged().Broadcast(Modifier->GetType(), Adjustment);
}

void ATartarusTreasureHuntGameState::ResetGameModifiers()
{
	//for (auto& Modifier : GameModifiers)
	//{
	//	Modifier.Value = 0;
	//}
	GameModifiers.Empty();

	OnGameModifiersChanged().Broadcast(EGameModifier::None, 0.0f);
}
#pragma endregion
