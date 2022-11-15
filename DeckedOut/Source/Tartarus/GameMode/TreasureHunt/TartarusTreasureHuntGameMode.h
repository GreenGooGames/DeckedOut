// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/TartarusGameMode.h"

#include "TartarusTreasureHuntGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusTreasureHuntGameMode : public ATartarusGameMode
{
	GENERATED_BODY()
	
public:
	void StartTreasureHunt();
	void StopTreasureHunt();

	bool IsTreasureHuntInProgress() const { return bIsTreaseHuntInProgress; }

private:
	bool bIsTreaseHuntInProgress = false;
};
