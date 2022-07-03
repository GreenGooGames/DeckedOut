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

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Id = INVALID_ItemId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name = FText();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItem> Blueprint = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> DisplayTexture;

	bool IsDataValid() const;
};