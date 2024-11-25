// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GleipnirLootTable.generated.h"

class UGleipnirItemAsset;

USTRUCT(BlueprintType)
struct FGleipnirLootTableEntry
{
	GENERATED_BODY()

public:
	// Item represented by this entry.
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UGleipnirItemAsset> ItemHandle = nullptr;

	// Weight of the item, negative value means guarenteed.
	UPROPERTY(EditDefaultsOnly)
	int32 Weight = 0;
};

/**
 * 
 */
UCLASS(BlueprintType)
class GLEIPNIR_API UGleipnirLootTable : public UDataAsset
{
	GENERATED_BODY()

public:
	int32 GetMaxCandidates() const { return MaxCandidates; }
	const TArray<FGleipnirLootTableEntry>& GetLootCandidates() const {return Candidates;}

protected:
	// Maximum number of candidates when loot is requested (Excluding guarenteed loot).
	UPROPERTY(EditDefaultsOnly)
	int32 MaxCandidates = 0;

	// Loot candidates.
	UPROPERTY(EditDefaultsOnly)
	TArray<FGleipnirLootTableEntry> Candidates = TArray<FGleipnirLootTableEntry>();
};
