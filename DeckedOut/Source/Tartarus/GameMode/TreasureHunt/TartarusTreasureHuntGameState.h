// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "TartarusTreasureHuntGameState.generated.h"

enum class EGameModifier : uint8;
class UTartarusGameModifier;

UENUM()
enum class ETreasureHuntState : uint8
{
	Active,
	Inactive,
	Paused,
};

DECLARE_EVENT_TwoParams(ATartarusTreasureHuntGameState, FRunningStateChanged, ETreasureHuntState /*OldState*/, ETreasureHuntState /*NewState*/);
DECLARE_EVENT_TwoParams(ATartarusTreasureHuntGameState, FGameModifiersChanged, const EGameModifier /*ModifierType*/, const float /*Adjustment*/);

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

private:
	ETreasureHuntState TreasureHuntState = ETreasureHuntState::Inactive;
	FRunningStateChanged RunningStateChangedEvent = FRunningStateChanged();
#pragma endregion

#pragma region GameModifiers
public:
	const TMap<TObjectPtr<const UTartarusGameModifier>, float>& GetWeightedGameModifiers() const { return GameModifiers; };

	/*
	* Modifiers applied to this instance of the game. Usually set trough the chosen card deck.
	* Return: Struct containing all modifiers applied to the game.
	*/
	const TPair<TObjectPtr<const UTartarusGameModifier>, float>* FindGameModifier(const EGameModifier ModifierType);

	/*
	* Updates the given game modifier with the provided adjustment.
	*/
	void EditGameModifier(const UTartarusGameModifier* const Modifier, const float Adjustment);

	/*
	* Resets the Game Modifiers to their default values.
	*/
	void ResetGameModifiers();

	FGameModifiersChanged& OnGameModifiersChanged() { return GameModifiersChangedEventHandle; }

private:
	TMap<TObjectPtr<const UTartarusGameModifier>, float> GameModifiers;
	FGameModifiersChanged GameModifiersChangedEventHandle = FGameModifiersChanged();
#pragma endregion
};
