// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableItem.h"

#include "Item/Equipable/TartarusEquipableManager.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"

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
