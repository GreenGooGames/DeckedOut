// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/SphereComponent.h"
#include "DeckedOut/Components/Inventory/InventoryComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	Volume = CreateDefaultSubobject<USphereComponent>(FName("Volume"), false);

	SetRootComponent(Volume);
	Mesh->SetupAttachment(Volume);
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
				Destroy();

				return true;
			}
		}
	}

	return false;
}
#pragma endregion
