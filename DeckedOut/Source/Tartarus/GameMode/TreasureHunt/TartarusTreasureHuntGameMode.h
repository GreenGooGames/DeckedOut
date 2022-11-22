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
	// Compass to be gifted to the players when a treasure hunt starts.
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle StarterCompass;

	// Number of compasses to gift when the treasure hunt starts per player.
	UPROPERTY(EditDefaultsOnly)
		int32 NumGiftCompasses = 0;

	// Slot to auto-equip the compass to when received.
	UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = "EEquipmentSlot"))
		uint8 AutoEquipCompassSlotMask;

	void GiftStarterItems(AController* const PlayerController) const;
};
