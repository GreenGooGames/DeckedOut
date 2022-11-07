// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "Item/TartarusItemBase.h"
#include "System/TartarusHelpers.h"

#include "TartarusItemData.generated.h"

// General information of an item.
USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Unique Id to identify the item.
	UPROPERTY(EditDefaultsOnly)
		int32 UniqueId = FTartarusHelpers::InvalidItemId;

	// Name to represent the item.
	UPROPERTY(EditDefaultsOnly)
		FText Name = FText();
	
	// Description to provide information about the item.
	UPROPERTY(EditDefaultsOnly)
		FText Description = FText();

	// Reference blueprint to instaniate when spawned in the world.
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<ATartarusItemBase> Blueprint = nullptr;

	// Reference texture to represent the item in UI.
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UTexture2D> DisplayTexture = nullptr;
};

// Data containing item information in the inventory.
USTRUCT()
struct FInventoryItemEntry
{
	GENERATED_BODY()

public:
	int32 ReferenceId = FTartarusHelpers::InvalidItemId;
	int32 Quantity = 0;
};
