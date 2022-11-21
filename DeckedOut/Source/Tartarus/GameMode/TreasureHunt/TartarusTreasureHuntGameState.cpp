// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"

void ATartarusTreasureHuntGameState::ChangeTreasureHuntState(const ETreasureHuntState NewState)
{
	TreasureHuntState = NewState;
}

bool ATartarusTreasureHuntGameState::IsTreasureHuntActive() const
{
	return TreasureHuntState == ETreasureHuntState::Active;
}
