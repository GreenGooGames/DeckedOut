// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableManager.h"

#include "GameFramework/Character.h"
#include "Item/Equipable/TartarusEquipableInterface.h"
#include "Logging/TartarusLogChannels.h"

// Sets default values for this component's properties
UTartarusEquipableManager::UTartarusEquipableManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTartarusEquipableManager::BeginPlay()
{
	Super::BeginPlay();

	// Check if the slots are setup.
	if (SocketMapping.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Equipment mapping is not setup!"), __FUNCTION__);
		return;
	}

	for (TPair<EEquipmentSlot, FName> SlotToNamePair : SocketMapping)
	{
		EquipmentSlots.Add(SlotToNamePair.Key, nullptr);
	}
}


bool UTartarusEquipableManager::Equip(TObjectPtr<AActor> ToEquip, const EEquipmentSlot SlotName)
{
	// Is the object to equip valid?
	if (!IsValid(ToEquip))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Object was invalid!"), __FUNCTION__);
		return false;
	}

	// Is this an equipable object?
	ITartarusEquipableInterface* const EquipableInterface = Cast<ITartarusEquipableInterface>(ToEquip);
	if (EquipableInterface == nullptr)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Object is not an equipable object!"), __FUNCTION__);
		return false;
	}

	// Is the requested slot setup?
	if (!EquipmentSlots.Contains(SlotName))
	{
		return false;
	}

	// Is the requested slot available?
	const EEquipmentSlot AvailableSlot = GetAvailableSlot(SlotName);
	if (AvailableSlot == EEquipmentSlot::None)
	{
		return false;
	}

	// Equip the Object.
	EquipmentSlots[AvailableSlot] = ToEquip;
	const ACharacter* const Owner = Cast<ACharacter>(GetOwner());

	if (Owner)
	{
		USkeletalMeshComponent* const AttachComponent = Owner->GetMesh();

		if (AttachComponent)
		{
			const FName& SocketName = SocketMapping[AvailableSlot];

			if (SocketName != NAME_None)
			{
				const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
				ToEquip->AttachToComponent(AttachComponent, AttachmentRules, SocketName);

				EquipableInterface->OnEquipped();
			}
		}
	}

	return true;
}

bool UTartarusEquipableManager::Unequip(TObjectPtr<AActor> ToUnequip)
{
	if (!IsValid(ToUnequip))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to unequip: Object was invalid!"), __FUNCTION__);
		return false;
	}

	for (TPair<EEquipmentSlot, TWeakObjectPtr<AActor>>& EquipmentSlot : EquipmentSlots)
	{
		if (ToUnequip == EquipmentSlot.Value)
		{
			const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
			
			EquipmentSlot.Value = nullptr;
			ToUnequip->DetachFromActor(DetachmentRules);
			ToUnequip->SetActorLocation(ToUnequip->GetActorLocation() + ToUnequip->GetActorForwardVector() * 100);

			ITartarusEquipableInterface* const EquipableInterface = Cast<ITartarusEquipableInterface>(ToUnequip);
			if (EquipableInterface)
			{
				EquipableInterface->OnUnequipped();
			}

			return true;
		}
	}

	return false;
}

EEquipmentSlot UTartarusEquipableManager::GetAvailableSlot(const EEquipmentSlot RequestedSlot) const
{
	for (TPair<EEquipmentSlot, TWeakObjectPtr<AActor>> EquipmentSlot : EquipmentSlots)
	{
		const uint8 Request = uint8(RequestedSlot);
		const uint8 Current = uint8(EquipmentSlot.Key);
		const uint8 BitResult = Request & Current;

		if (BitResult == Request)
		{
			const EEquipmentSlot FoundSlot = static_cast<EEquipmentSlot>(BitResult);

			if (!IsValid(EquipmentSlots[FoundSlot].Get()))
			{
				return FoundSlot;
			}
		}
	}

	return EEquipmentSlot::None;
}
