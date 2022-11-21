// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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

protected:
	// Item to be gifted when a player starts the game.
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle GiftItemRow;

	void GiftStarterItems(AController* const PlayerController) const;
};
