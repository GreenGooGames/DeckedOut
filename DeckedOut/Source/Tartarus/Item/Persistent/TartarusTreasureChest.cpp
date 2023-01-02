// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Persistent/TartarusTreasureChest.h"

#include "Audio/TartarusNoiseSourceComponent.h"
#include "Player/TartarusPlayerCharacter.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Loot/TartarusLootComponent.h"
#include "Item/TartarusItemBase.h"
#include "logging/TartarusLogChannels.h"

#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"

ATartarusTreasureChest::ATartarusTreasureChest()
{
	LootComponent = CreateDefaultSubobject<UTartarusLootComponent>(TEXT("LootComponent"));

	NoiseSourceComponent = CreateDefaultSubobject<UTartarusNoiseSourceComponent>(TEXT("Noise Source Component"));
}

void ATartarusTreasureChest::HandleLootDropped(FGuid RequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedLoot)
{
	LootedEvent.Broadcast(this);
}

#pragma region TartarusInteractableTargetInterface
bool ATartarusTreasureChest::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusTreasureChest::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	ATartarusPlayerCharacter* const Player = Cast<ATartarusPlayerCharacter>(InstigatorController->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Interaction failed: Instigator was not a player!"), *FString(__FUNCTION__));
		return false;
	}

	// Remove the compass from the inventory.
	UTartarusInventoryComponent* const Inventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		return false;
	}

	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();
	if (!IsValid(TreasureSubsystem))
	{
		return false;
	}

	const FGuid LinkedKeyInventoryStackId = TreasureSubsystem->GetTreasureKey(this);
	if (!LinkedKeyInventoryStackId.IsValid())
	{
		return false;
	}

	// Retrieve the number of required keys, fail if there are not enough keys.
	const bool bHasRetrievedItem = Inventory->RetrieveItem(LinkedKeyInventoryStackId, NumKeysRequired);
	if (!bHasRetrievedItem)
	{
		return false;
	}

	// Droploot.
	FDropLootRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusTreasureChest::HandleLootDropped);

	LootComponent->AsyncRequestDropLoot(GetActorTransform(), OnRequestCompleted);

	// Play sound of the chest opening.
	NoiseSourceComponent->GenerateNoise(LootingSound, GetActorLocation());

	return true;
}
#pragma endregion