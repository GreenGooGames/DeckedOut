// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ruleset/TartarusRuleSubsystem.h"

#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Clank/TartarusClankSubsystem.h"
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
	UTartarusClankSubsystem* const ClankSubsystem = GetWorld()->GetSubsystem<UTartarusClankSubsystem>();
	if (IsValid(ClankSubsystem))
	{
		ClankSubsystem->OnClankLevelChanged().AddUObject(this, &UTartarusRuleSubsystem::HandleClankLevelChanged);
		HandleClankLevelChanged(0);
	}
}

void UTartarusRuleSubsystem::DisableRuleset()
{
	UTartarusClankSubsystem* const ClankSubsystem = GetWorld()->GetSubsystem<UTartarusClankSubsystem>();
	if (IsValid(ClankSubsystem))
	{
		ClankSubsystem->OnClankLevelChanged().RemoveAll(this);
	}

	Ruleset->Reset(GetWorld());
}

void UTartarusRuleSubsystem::HandleClankLevelChanged(int32 ClankLevel)
{
	// Check if the change in clank level caused an advancement in the ruleset.
	const bool bStageChanged = Ruleset->ActivateStage(GetWorld(), ActiveRulesetStage, ClankLevel);

	if (!bStageChanged)
	{
		return;
	}
}
