// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TartarusTreasureHuntGameState.generated.h"

UENUM()
enum class ETreasureHuntState : uint8
{
	Active,
	Inactive,
	Paused,
};

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusTreasureHuntGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Changes the state of the treasure hunt if possible.
	void ChangeTreasureHuntState(const ETreasureHuntState NewState);

	/*
	* Checks if the treasure hunt is active.
	* Return: True if the treasure hunt is ongoing. False if the treasure hunt is inactive or paused.
	*/
	bool IsTreasureHuntActive() const;

protected:
	
private:
	ETreasureHuntState TreasureHuntState = ETreasureHuntState::Inactive;
};
