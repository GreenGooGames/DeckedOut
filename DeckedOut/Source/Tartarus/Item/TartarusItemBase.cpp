// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TartarusItemBase.h"

#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"

// Sets default values
ATartarusItemBase::ATartarusItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	SetRootComponent(Mesh);
}

#pragma region TartarusInteractableTargetInterface
bool ATartarusItemBase::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusItemBase::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// As base behavior, any item interacted with is stored into the instigator inventory.
	UTartarusInventoryComponent* const InstigatorInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!InstigatorInventory)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Controller has no inventory!"), __FUNCTION__);
		return false;
	}

	// [Koen Goossens] TODO: Magic Number 1, setup a stacksize as parameter.
	const bool bIsStored = InstigatorInventory->StoreItem(ReferenceId, 1);

	return bIsStored;
}
#pragma endregion
