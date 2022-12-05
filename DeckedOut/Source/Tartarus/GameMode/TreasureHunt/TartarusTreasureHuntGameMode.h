// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameMode/TartarusGameMode.h"

#include "TartarusTreasureHuntGameMode.generated.h"

class UTartarusRuleset;

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

#pragma region StarterGifts
protected:
	// TreasureKey item handle to be gifted to the players when a treasure hunt starts.
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle StarterTreasureKeyHandle;

	// Stack count of the treasure key to gift. Should always be 1 for a unique.
	const int32 GiftTreasureKeyStackCount = 1;

	// Inventory ID of the gifted treasure key to remove if the player leaves the dungeon without using it.
	FGuid GiftedTreasureKeyInventoryId;

	/*
	* Stores starter items into the player inventory.
	* Return: True if the items are given. False if the items could not be gifted. (ex: lack of inventory space)
	*/
	bool GiftStarterItems(const AController* const PlayerController);
#pragma endregion

#pragma region RuleSet
protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusRuleset> Ruleset;

	void HandleClankLevelChanged(int32 ClankLevel);
	void EnableRuleset();
	void DisableRuleset();

private:
	FDelegateHandle HandleClankLevelChangedDelegateHandle = FDelegateHandle();
	int32 ActiveClankStage = INDEX_NONE;
#pragma endregion
};
