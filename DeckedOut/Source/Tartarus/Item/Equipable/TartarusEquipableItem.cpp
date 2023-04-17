// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableItem.h"

#include "Components/StaticMeshComponent.h"

#pragma region EquipableInterface
void ATartarusEquipableItem::OnEquipped(AActor* const EquippedActor)
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
