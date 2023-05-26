// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusGameModifiersWidget.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Engine/World.h"
#include "CommonListView.h"
#include "UI/Foundation/TartarusGameModifierEntryData.h"
#include "Gameplay/Ruleset/TartarusGameModifier.h"

void UTartarusGameModifiersWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		return;
	}

	GameState->OnGameModifiersChanged().AddUObject(this, &UTartarusGameModifiersWidget::HandleGameModifiersChanged);
	Refresh();
}

void UTartarusGameModifiersWidget::Refresh()
{
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		return;
	}

	if (!IsValid(GameModifierList))
	{
		return;
	}

	GameModifierList->ClearListItems();

	TMap<TObjectPtr<const UTartarusGameModifier>, float> WeightedGameModifiers = GameState->GetWeightedGameModifiers();
	for (auto& WeightedModifier : WeightedGameModifiers)
	{
		UTartarusGameModifierEntryData* const EntryData = NewObject<UTartarusGameModifierEntryData>();
		if (!IsValid(EntryData))
		{
			continue;
		}

		EntryData->ModifierName = WeightedModifier.Key->GetName();
		EntryData->ModifierTooltip = WeightedModifier.Key->GetDescription();
		EntryData->ModifierWeight = WeightedModifier.Value;

		GameModifierList->AddItem(EntryData);
	}
}

void UTartarusGameModifiersWidget::HandleGameModifiersChanged(const EGameModifier Modifier, const float Adjustment)
{
	Refresh();
}
