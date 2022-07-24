// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "ItemData.generated.h"

typedef TVariant<FVector> ItemUniqueDataType;
constexpr int32 INVALID_ItemId = -1;

class AItem;

//Holds the data that is to be defined in a data asset.
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 Id = INVALID_ItemId;

	UPROPERTY(EditDefaultsOnly)
	FText Name = FText();
	UPROPERTY(EditDefaultsOnly)
	FText Description = FText();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AItem> Blueprint = nullptr;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> DisplayTexture = nullptr;

	bool IsDataValid() const;
};

// Holds the data for an item in the inventory.
USTRUCT(BlueprintType)
struct FItemData_Inventory
{
	GENERATED_BODY()

public:
	int32 Id = INVALID_ItemId;

	FText Name = FText();
	FText Description = FText();

	TObjectPtr<UTexture2D> DisplayTexture;

	int32 StackSize = -1;
	TMap<FString, ItemUniqueDataType> UniqueData = TMap<FString, ItemUniqueDataType>();

	FItemData_Inventory() {};
	FItemData_Inventory(const FItemData& ItemData, const int32 Amount);
};