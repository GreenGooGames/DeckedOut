// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Engine/DataTable.h"

#include "TartarusLootTableDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLootTableEntry
{
	GENERATED_BODY()

public:
	// Reference to the item to include in the loot.
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle ItemHandle = FDataTableRowHandle();

	// Weight of the item, negative value means guarenteed.
	UPROPERTY(EditDefaultsOnly)
		int32 Weight = 0;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TARTARUS_API UTartarusLootTableDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/*
	* Picks random loot from the candidates.
	* Return: Candidates to loot.
	*/
	TArray<FDataTableRowHandle> GetLoot();

protected:
	// Maximum number of candidates when loot is requested (Excluding guarenteed loot).
	UPROPERTY(EditDefaultsOnly)
		int32 MaxCandidates = 0;

	// Loot candidates.
	UPROPERTY(EditDefaultsOnly)
		TArray<FLootTableEntry> Candidates;
};
