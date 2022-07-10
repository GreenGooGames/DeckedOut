// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Components/Loot/LootComponent.h"

#include "DeckedOut/World/Items/ItemManagerSubsystem.h"

// Sets default values for this component's properties
ULootComponent::ULootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool ULootComponent::DropLoot() const
{
	// Get a random drop from the lootable.
	UDataTable* const LootTableRaw = LootTable.Get();

	if (IsValid(LootTableRaw))
	{
		// Get a random entry from the LootTable.
		TArray<FLootTableRow*> LootDrops;
		FString ContextString = "";
		LootTableRaw->GetAllRows<FLootTableRow>(ContextString, LootDrops);

		const int32 RandomIndex = rand() % (LootDrops.Num() - 1);
		const FLootTableRow* const RandomRow = LootDrops[RandomIndex];

		if (RandomRow)
		{
			const UItemDataAsset* const ItemDataAsset = RandomRow->Item.LoadSynchronous();
			
			if (IsValid(ItemDataAsset))
			{
				// Spawn the loot in the world trough the itemannager subsystem.
				UItemManagerSubsystem* const ItemManagerSubsystem = GetWorld()->GetSubsystem<UItemManagerSubsystem>();

				if (IsValid(ItemManagerSubsystem))
				{
					const FTransform& SpawnTransform = GetOwner()->GetActorTransform();

					const bool bIsLootSpawned = ItemManagerSubsystem->SpawnItem(ItemDataAsset->ItemData.Id, SpawnTransform);

					return bIsLootSpawned;
				}
			}
		}
	}

	return false;
}
