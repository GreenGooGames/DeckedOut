// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/World/Treasure/TreasureChest.h"

#include "DeckedOut/Components/Inventory/InventoryComponent.h"
#include "DeckedOut/Components/Loot/LootComponent.h"
#include "DeckedOut/World/Items/Item.h"
#include "DeckedOut/World/Items/ItemDataAsset.h"

// Sets default values
ATreasureChest::ATreasureChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	LootComponent = CreateDefaultSubobject<ULootComponent>(FName("LootComponent"), false);

	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
}

#pragma region IInteractableInterface
bool ATreasureChest::IsInteractable() const
{
	return bIsInteractable && WasRecentlyRendered();
}

bool ATreasureChest::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// Check if the instigator has a compass in their inventory.
	const AController* const InstigatorControllerRaw = InstigatorController.Get();

	if (IsValid(InstigatorControllerRaw))
	{
		UInventoryComponent* const InstigatorInventory = InstigatorControllerRaw->FindComponentByClass<UInventoryComponent>();

		if (IsValid(InstigatorInventory))
		{
			// Take the compass from the instigator.
			int32 NumRetrievedItems = 0;

			for (const TSoftObjectPtr<UItemDataAsset> Key : CompatibleKeys)
			{
				const UItemDataAsset* const KeyRaw = Key.LoadSynchronous();

				if (IsValid(KeyRaw))
				{
					// [Koen Goossens] TODO: First check if there are enough keys, for now the number is set to 1.
					// [Koen Goossens] TODO: Check for the correct key, requires implementation to find a specific item in the inventory trough its unique data.
					TMap<FString, ItemUniqueDataType> KeyUniqueData;
					if (InstigatorInventory->RetrieveItems(KeyRaw->ItemData.Id, NumKeysRequired, NumRetrievedItems, KeyUniqueData))
					{
						break;
					}
				}
			}

			if (NumRetrievedItems > 0)
			{
				// Drop the loot.
				if (IsValid(LootComponent))
				{
					bIsInteractable = false;

					LootComponent->DropLoot();

					// [Koen Goossens] TODO: Destroy this actor trough the Object manager.
					Destroy();

					return true;
				}
			}
		}
	}

	return false;
}
#pragma endregion

#pragma region ICompassTargetInterface

#pragma endregion
