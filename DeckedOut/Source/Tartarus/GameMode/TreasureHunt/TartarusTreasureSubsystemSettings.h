// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Item/Persistent/TartarusTreasureChest.h"

#include "TartarusTreasureSubsystemSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Treasure Subsystem Settings"))
class TARTARUS_API UTartarusTreasureSubsystemSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// The class of the treasure to spawn.
	UPROPERTY(Config, EditDefaultsOnly)
		TSoftClassPtr<ATartarusTreasureChest> TreasureClass;

	// All keys that can open treasure.
	UPROPERTY(Config, EditDefaultsOnly)
		TArray<FDataTableRowHandle> TreasureKeys;
};
