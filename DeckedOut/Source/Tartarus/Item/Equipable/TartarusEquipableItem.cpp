// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableItem.h"

#include "Item/Equipable/TartarusEquipableManager.h"
#include "Logging/TartarusLogChannels.h"

bool ATartarusEquipableItem::HandlePickedup(const TObjectPtr<AController> InstigatorController)
{
	// Add the Item to the inventory.
	const bool bIsStored = Super::HandlePickedup(InstigatorController);

	if (!bIsStored)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Item was not added to the inventory!"), __FUNCTION__);
		return false;
	}

	UTartarusEquipableManager* const EquipableManager = InstigatorController->GetPawn()->FindComponentByClass<UTartarusEquipableManager>();

	if (!IsValid(EquipableManager))
	{
		return false;
	}

	const bool IsEquipped = EquipableManager->Equip(this, EEquipmentSlot::LeftHand);

	return IsEquipped;
}

#pragma region EquipableInterface
void ATartarusEquipableItem::OnEquipped()
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetEnableGravity(false);
}

void ATartarusEquipableItem::OnUnequipped()
{
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetEnableGravity(true);
}
#pragma endregion
