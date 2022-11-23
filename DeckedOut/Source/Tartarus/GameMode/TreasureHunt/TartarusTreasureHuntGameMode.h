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
	void StartTreasureHunt() const;
	void StopTreasureHunt() const;

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

	/*
	* Stores starter items into the player inventory.
	* Return: True if the items are given. False if the items could not be gifted. (ex: lack of inventory space)
	*/
	bool GiftStarterItems(const AController* const PlayerController) const;
};
