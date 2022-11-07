// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"

#include "TartarusItemSubsystemSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Item Subsystem Settings"))
class TARTARUS_API UTartarusItemSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// Datatable reference that contains all items.
	UPROPERTY(Config, EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> ItemDataTable;

};
