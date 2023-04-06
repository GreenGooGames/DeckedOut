// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cards/TartarusCard.h"

#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"

void UTartarusCard::ApplyModifiers(ATartarusTreasureHuntGameState* const GameState) const
{
	for (TPair<EGameModifier, float> Modifier : Modifiers)
	{
		GameState->EditGameModifier(Modifier.Key, Modifier.Value);
	}
}

void UTartarusCard::RemoveModifiers(ATartarusTreasureHuntGameState* const GameState) const
{
	for (TPair<EGameModifier, float> Modifier : Modifiers)
	{
		GameState->EditGameModifier(Modifier.Key, -Modifier.Value);
	}
}
