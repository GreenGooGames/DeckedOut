// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"

void ATartarusTreasureHuntGameMode::StartTreasureHunt()
{
	bIsTreaseHuntInProgress = true;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Treasure Hunt Start!")));
#endif
}

void ATartarusTreasureHuntGameMode::StopTreasureHunt()
{
	bIsTreaseHuntInProgress = false;

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Treasure Hunt Stop!")));
#endif
}
