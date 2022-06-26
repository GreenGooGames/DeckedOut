// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "ItemData.generated.h"

constexpr int32 INVALID_ItemId = -1;

class AItem;

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	int32 Id = INVALID_ItemId;
	FText Name = FText();

	TSubclassOf<AItem> Blueprint = nullptr;
	FText Description = FText();

	bool IsDataValid() const;
};