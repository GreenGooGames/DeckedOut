// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Audio/TartarusSoundData.h"
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TartarusClankSubsystem.generated.h"

enum class ETreasureHuntState : uint8;
enum class ENoiseLevel : uint8;


DECLARE_EVENT_OneParam(UTartarusClankSubsystem, FClankLevelChanged, int32 /*ClankLevel*/);

/**
 * Clank gets generated passively and actively on actions.
 */
UCLASS()
class TARTARUS_API UTartarusClankSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	// Increases the clank level based on the amount of noise generated.
	void GenerateClank(const ENoiseLevel NoiseLevel);

	// Event fired each time the clank level changes.
	FClankLevelChanged& OnClankLevelChanged() { return ClankLevelChangedEvent; }

protected:
	// TODO: Create a subsystem setting to set this value.
	int32 BaseClankGenerationLevel = 1;

	// TODO: Create a subsystem setting to set this value.
	float ClankGenerationInterval = 3.0f;

	// TODO: Create a subsystem setting to set this value.
	ENoiseLevel PassiveClankVolume = ENoiseLevel::Faint;

	int32 ClankLevel = 0;

	// Resets the ClankLevel back to 0.
	void ResetClank();

	// Fired when the GameState changes, Spawns/Despawns all treasures.
	void HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState);

	// Increases the clank level based on the passive noise generated.
	void GeneratePassiveClank();

	ENoiseLevel ApplyGameModifers(const ENoiseLevel NoiseLevel) const;

private:
	FClankLevelChanged ClankLevelChangedEvent = FClankLevelChanged();
	FTimerHandle PassiveClankGenerationTimerHandle;
};
