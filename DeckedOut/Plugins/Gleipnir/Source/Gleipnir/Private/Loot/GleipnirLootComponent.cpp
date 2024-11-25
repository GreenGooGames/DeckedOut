// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/GleipnirLootComponent.h"

#include "Engine/World.h"

#include "Loot/GleipnirLootTable.h"
#include "Items/GleipnirItemSubsystem.h"

// Sets default values for this component's properties
UGleipnirLootComponent::UGleipnirLootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UGleipnirLootComponent::DropLoot(const FTransform& Transform) const
{
	const UGleipnirItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UGleipnirItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		return false;
	}

	const TArray<TSoftObjectPtr<UGleipnirItemAsset>> Loot = GetLoot();

	const TArray<AActor*> SpawnedItems = ItemSubsystem->SpawnItems(Loot, Transform);

	return !SpawnedItems.IsEmpty();
}

TArray<TSoftObjectPtr<UGleipnirItemAsset>> UGleipnirLootComponent::GetLoot() const
{
	TArray<TSoftObjectPtr<UGleipnirItemAsset>> Loot;

	const UGleipnirLootTable* const LootTableRaw = LootTable.LoadSynchronous();
	if (!IsValid(LootTableRaw))
	{
		return Loot;
	}

	const TArray<FGleipnirLootTableEntry>& Candidates = LootTableRaw->GetLootCandidates();
	int32 TotalWeight = 0;

	// Gather all potential candidates.
	TMap<int32, int32> LootCandidates; // <Index, MaxWeight>

	for (int32 i = 0; i < Candidates.Num(); i++)
	{
		const FGleipnirLootTableEntry& Candidate = Candidates[i];

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
	for (int32 i = 0; i < LootTableRaw->GetMaxCandidates() - 1; i++)
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


