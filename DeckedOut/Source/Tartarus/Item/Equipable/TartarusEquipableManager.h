// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TartarusEquipableManager.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class EEquipmentSlot : uint8
{
	None = 0x00,
	LeftHand = 0x01,
	RightHand = 0x02,
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusEquipableManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTartarusEquipableManager();

protected:
	virtual void BeginPlay() override;

public:
	/*
	* Equips an actor to the owner.
	* Return: True, actor was equipped. False, The actor could not be equipped.
	*/
	bool Equip(TObjectPtr<AActor> ToEquip, const EEquipmentSlot SlotName);

	/*
	* Unequips the actor if it is equipped.
	* Return: True, the actor was unequipped. False, Failed to unequip the actor.
	*/
	bool Unequip(TObjectPtr<AActor> ToUnequip);

protected:
	// Map that identifies which Skeleton Socket corresponds to a slot.
	UPROPERTY(EditDefaultsOnly)
		TMap<EEquipmentSlot, FName> SocketMapping;

	TMap<EEquipmentSlot, TWeakObjectPtr<AActor>> EquipmentSlots;

	/*
	* Finds a slot that is available to equip on.
	* Return: A slot that is available to equip on, EEquipmentSlot::None if no slot is available.
	*/
	EEquipmentSlot GetAvailableSlot(const EEquipmentSlot RequestedSlot) const;

};
