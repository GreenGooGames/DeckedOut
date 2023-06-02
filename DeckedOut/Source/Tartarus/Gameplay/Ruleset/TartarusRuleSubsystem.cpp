// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleSubsystem.h"

#include "Engine/World.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Ruleset/TartarusRuleset.h"

void UTartarusRuleSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	ATartarusTreasureHuntGameState* const GameState = InWorld.GetGameState<ATartarusTreasureHuntGameState>();

	if (!IsValid(GameState))
	{
		return;
	}

	GameState->OnRunningStateChanged().AddUObject(this, &UTartarusRuleSubsystem::HandleGameRunningStateChanged);
}

void UTartarusRuleSubsystem::SetRuleset(UTartarusRulesetData* const RulesetData)
{
	if (IsValid(Ruleset))
	{
		Ruleset->Deactivate();
	}

	Ruleset = NewObject<UTartarusRuleset>();
	Ruleset->Initialize(RulesetData);
}

void UTartarusRuleSubsystem::HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState)
{
	if (OldState == NewState)
	{
		return;
	}

	switch (NewState)
	{
	case ETreasureHuntState::Active:
	{
		EnableRuleset();
		break;
	}
	case ETreasureHuntState::Inactive:
	{
		DisableRuleset();
		break;
	}
	// Intentional fall-trough, no specified behavior for a paused state.
	case ETreasureHuntState::Paused:
	default:
		break;
	}
}

void UTartarusRuleSubsystem::EnableRuleset()
{
	Ruleset->Activate(GetWorld());
}

void UTartarusRuleSubsystem::DisableRuleset()
{
	Ruleset->Deactivate();
}
