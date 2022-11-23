// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/TartarusPickup.h"

#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"

ATartarusPickup::ATartarusPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	SetRootComponent(Mesh);
}

bool ATartarusPickup::HandlePickedup(const TObjectPtr<AController> InstigatorController)
{
	// As base behavior, any item interacted with is stored into the instigator inventory.
	UTartarusInventoryComponent* const InstigatorInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(InstigatorInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Controller has no inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// [Koen Goossens] TODO: Magic Number 1, setup a stacksize as parameter.
	const bool bIsStored = InstigatorInventory->StoreItem(GetItemId(), 1).IsValid();

	if (!bIsStored)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Could not store the item in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	Destroy();

	return true;;
}

#pragma region TartarusInteractableTargetInterface
bool ATartarusPickup::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusPickup::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	if (!IsInteractable())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Item is not interactable!"), *FString(__FUNCTION__));
		return false;
	}

	if (!IsValid(InstigatorController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Controller is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const bool bSuccess = HandlePickedup(InstigatorController);

	return bSuccess;
}
#pragma endregion
