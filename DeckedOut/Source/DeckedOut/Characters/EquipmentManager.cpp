// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Characters/EquipmentManager.h"

// Sets default values for this component's properties
UEquipmentManager::UEquipmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEquipmentManager::BeginPlay()
{
	Super::BeginPlay();

	// Check if the slots are setup.
	if (SocketMapping.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Has an equipment manager but no mapping is setup"), *GetName());
	}

	for (TPair<EEquipmentSlot, FName> SlotToNamePair : SocketMapping)
	{
		EquipedObjects.Add(SlotToNamePair.Key, nullptr);
	}
}

bool UEquipmentManager::Equip(TSoftObjectPtr<AActor>& Actor, const EEquipmentSlot RequestedSlot)
{
	// Is the object to equip valid?
	AActor* const ToEquipActor = Actor.Get();
	if (!IsValid(ToEquipActor))
	{
		return false;
	}

	// Is the requested slot setup?
	if (!EquipedObjects.Contains(RequestedSlot))
	{
		return false;
	}

	// Is the requested slot available?
	EEquipmentSlot AvailableSlot = GetAvailableSlot(RequestedSlot);
	if (AvailableSlot == EEquipmentSlot::None)
	{
		return false;
	}

	// Equip the Object.
	EquipedObjects[AvailableSlot] = Actor;
	AActor* const AttachParent = GetOwner();
	Actor->AttachToActor(AttachParent, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketMapping[AvailableSlot]);

	return true;
}

bool UEquipmentManager::Unequip(TSoftObjectPtr<AActor>& Actor)
{
	bool bHasUnequipedItem = false;

	for (TPair<EEquipmentSlot, TSoftObjectPtr<AActor>>& EquipmentSlotData : EquipedObjects)
	{
		if (Actor == EquipmentSlotData.Value)
		{
			EquipmentSlotData.Value = nullptr;
			bHasUnequipedItem = true;
		}
	}

	return bHasUnequipedItem;
}

EEquipmentSlot UEquipmentManager::GetAvailableSlot(const EEquipmentSlot RequestedSlot) const
{
	for (TPair<EEquipmentSlot, TSoftObjectPtr<AActor>> EquipmentSlot : EquipedObjects)
	{
		const uint8 Request = uint8(RequestedSlot);
		const uint8 Current = uint8(EquipmentSlot.Key);
		const uint8 BitResult = Request & Current;

		if (BitResult == Request)
		{
			const EEquipmentSlot FoundSlot = static_cast<EEquipmentSlot>(BitResult);

			if (!IsValid(EquipedObjects[FoundSlot].Get()))
			{
				return FoundSlot;
			}
		}
	}

	return EEquipmentSlot::None;
}
