// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Loot/TartarusLootTableDataAsset.h"

TArray<FDataTableRowHandle> UTartarusLootTableDataAsset::GetLoot()
{
	TArray<FDataTableRowHandle> Loot;
	int32 TotalWeight = 0;

	// Gather all potential candidates.
	TMap<int32, int32> LootCandidates; // <Index, MaxWeight>

	for (int32 i = 0; i <= Candidates.Num() - 1; i++)
	{
		FLootTableEntry& Candidate = Candidates[i];

		if (Candidate.Weight < 0) // A guarenteed drop, add to the loot.
		{
			Loot.Add(Candidate.ItemHandle);
		}
		else // A weigted drop, store the index for later lookup.
		{
			TotalWeight += Candidate.Weight;
			LootCandidates.Add(i, TotalWeight);
		}
	}

	// Pick out random candidates to the amount of loot to spawn.
	for (int32 i = 0; i < MaxCandidates; i++)
	{
		const int32 RandomWeight = FMath::RandRange(0, TotalWeight);

		// Find the corresponding candidate to the weight.
		for (const TPair<int32, int32> Candidate : LootCandidates)
		{
			if (RandomWeight <= Candidate.Value)
			{
				Loot.Add(Candidates[Candidate.Key].ItemHandle);
				break;
			}
		}
	}

	return Loot;
}