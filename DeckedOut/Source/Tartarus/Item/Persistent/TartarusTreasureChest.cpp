// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Persistent/TartarusTreasureChest.h"

#include "Player/TartarusPlayerCharacter.h"
#include "Item/Equipable/Equipment/TartarusCompass.h"
#include "Item/Equipable/TartarusEquipableManager.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Loot/TartarusLootComponent.h"
#include "logging/TartarusLogChannels.h"

ATartarusTreasureChest::ATartarusTreasureChest()
{
	LootComponent = CreateDefaultSubobject<UTartarusLootComponent>(TEXT("LootComponent"));
}

bool ATartarusTreasureChest::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	ATartarusPlayerCharacter* const Player = Cast<ATartarusPlayerCharacter>(InstigatorController->GetPawn());

	if (!Player)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Interaction failed: Instigator was not a player!"), __FUNCTION__);
		return false;
	}

	// Detach the compass from the player.
	const bool bHasUnequippedCompass = Player->GetEquipableManager()->Unequip(LinkedCompass.Get());

	if (!bHasUnequippedCompass)
	{
		return false;
	}

	// Remove the compass from the inventory.
	UTartarusInventoryComponent* const Inventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		return false;
	}

	const bool bHasRetrievedItem = Inventory->RetrieveItem(LinkedCompass.Get()->GetReferenceId(), 1);

	if (!bHasRetrievedItem)
	{
		return false;
	}

	// Consume the compass used to open the treasure chest.
	if (IsValid(LinkedCompass.Get()))
	{
		LinkedCompass->Destroy();
	}

	// Droploot.
	FDropLootRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusTreasureChest::HandleLootDropped);

	LootComponent->AsyncRequestDropLoot(GetActorTransform(), OnRequestCompleted);

	return true;
}

void ATartarusTreasureChest::HandleLootDropped(FGuid RequestId, TWeakObjectPtr<ATartarusItemBase> SpawnedLoot)
{
	OnLooted.Broadcast(this);
}
