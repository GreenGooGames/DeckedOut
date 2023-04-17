// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "TartarusTreasureHuntGameState.generated.h"

UENUM(BlueprintType)
enum class EGameModifier : uint8
{
	None,
	Sneak,
	Silence
};

USTRUCT(BlueprintType)
struct FGameModifiers
{
	GENERATED_BODY()

public:
	// Has a chance to lower the volume of sound when generating clank.
	float SneakModifier = 0.0f;

	// Has a chance to completely nullify a sound when generating clank..
	float SilenceModifier = 0.0f;
};

UENUM()
enum class ETreasureHuntState : uint8
{
	Active,
	Inactive,
	Paused,
};

DECLARE_EVENT_TwoParams(ATartarusTreasureHuntGameState, FRunningStateChanged, ETreasureHuntState /*OldState*/, ETreasureHuntState /*NewState*/);

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusTreasureHuntGameState : public AGameStateBase
{
	GENERATED_BODY()

#pragma region TreasureHuntState
public:
	// Changes the state of the treasure hunt if possible.
	void ChangeTreasureHuntState(const ETreasureHuntState NewState);

	/*
	* Checks if the treasure hunt is active.
	* Return: True if the treasure hunt is ongoing. False if the treasure hunt is inactive or paused.
	*/
	bool IsTreasureHuntActive() const;

	/*
	* Event fired each time the running state of the treasure hunt changes.
	* Return: The event that gets fired when the state changes.
	*/
	FRunningStateChanged& OnRunningStateChanged() { return RunningStateChangedEvent; }

	/*
	* Modifiers applied to this instance of the game. Usually set trough the chosen card deck.
	* Return: Struct containing all modifiers applied to the game.
	*/
	const FGameModifiers& GetGameModifiers() const { return GameModifiers; }

	/*
	* Updates the given game modifier with the provided adjustment.
	*/
	void EditGameModifier(const EGameModifier Modifier, const float Adjustment);

	/*
	* Resets the Game Modifiers to their default values.
	*/
	void ResetGameModifiers() { GameModifiers = FGameModifiers(); };

protected:

private:
	ETreasureHuntState TreasureHuntState = ETreasureHuntState::Inactive;
	FRunningStateChanged RunningStateChangedEvent = FRunningStateChanged();
	FGameModifiers GameModifiers = FGameModifiers();
#pragma endregion
};
