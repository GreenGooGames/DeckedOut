// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TartarusRuleSubsystem.generated.h"

class UTartarusRuleset;
enum class ETreasureHuntState : uint8;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusRuleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	void SetRuleset(UTartarusRuleset* const NewRuleset) { Ruleset = NewRuleset; }

protected:
	// Fired when the GameState changes, Spawns/Despawns all treasures.
	void HandleGameRunningStateChanged(ETreasureHuntState OldState, ETreasureHuntState NewState);


	void HandleClankLevelChanged(int32 ClankLevel);
	void EnableRuleset();
	void DisableRuleset();

private:
	TObjectPtr<UTartarusRuleset> Ruleset;
	int32 ActiveRulesetStage = INDEX_NONE;
};
