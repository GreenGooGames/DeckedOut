// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/SphereComponent.h"
#include "DeckedOut/Components/Inventory/InventoryComponent.h"
#include "DeckedOut/World/Items/ItemManagerSubsystem.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);

	SetRootComponent(Mesh);

	SetupComponents(true);
}

void AItem::SetItemData(const FItemData& InItemData)
{
	ItemData = InItemData;
}

void AItem::SetupComponents(const bool bIsOnSpawn)
{
	Mesh->SetSimulatePhysics(bIsOnSpawn);
	Mesh->SetCollisionEnabled(bIsOnSpawn ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	Mesh->SetEnableGravity(bIsOnSpawn);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

}

#pragma region IInteractableInterface
bool AItem::IsInteractable() const
{
	return bIsInteractable && WasRecentlyRendered();
}

bool AItem::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	if (IsValid(InstigatorController.Get()))
	{
		if (UInventoryComponent* const Inventory = InstigatorController->FindComponentByClass<UInventoryComponent>())
		{
			if (Inventory->StoreItem(ItemData, GenerateUniqueData(), 1))
			{
				bIsInteractable = false;
				UItemManagerSubsystem* const ItemManagerSubsystem = GetWorld()->GetSubsystem<UItemManagerSubsystem>();

				if (IsValid(ItemManagerSubsystem))
				{
					ItemManagerSubsystem->DespawnItem(this);
				}

				return true;
			}
		}
	}

	return false;
}
#pragma endregion
