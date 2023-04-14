// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "TartarusItemData.generated.h"

class UTartarusItem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Artifact = 1,
	TreasureKey = 2,
	Card = 4,
	// Bitflags?
	// Enum = 8
	// enum = 16
	// Enum = 32
	// Enum = 64
	// Enum = 128
};

// General information of an item.
USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// The Asset that contains the data of the item.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusItem> ItemData = nullptr;
};
