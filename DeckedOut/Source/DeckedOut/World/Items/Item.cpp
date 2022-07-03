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

	Volume = CreateDefaultSubobject<USphereComponent>(FName("Volume"), false);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);

	SetRootComponent(Volume);
	Volume->SetSimulatePhysics(true);
	Volume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Volume->SetEnableGravity(true);
	Volume->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	Volume->InitSphereRadius(Mesh->Bounds.SphereRadius);

	Mesh->SetupAttachment(Volume);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::SetItemData(const FItemData& InItemData)
{
	ItemData = InItemData;
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
			if (Inventory->StoreItem(ItemData, 1))
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
