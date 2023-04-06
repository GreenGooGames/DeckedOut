// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cards/TartarusCardReader.h"

#include "Components/BoxComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"

// Sets default values
ATartarusCardReader::ATartarusCardReader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collider
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetSimulatePhysics(false);
	Collider->SetCollisionProfileName(FName("Trigger"), false);

	SetRootComponent(Collider);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetupAttachment(Collider);
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusCardReader::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusCardReader::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// Reset all modifiers.

	// Get the inventory of the instigator.
	const UTartarusInventoryComponent* const InstigatorInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	// Get all cards from the player inventory.
	InstigatorInventory->GetOverview(EInventoryType::Card);

	// Apply the effect of each card in the inventory.

	// TODO: Show a UI to have the player select cards from their inventory and only activate a few select ones.

	return true;
}
#pragma endregion
