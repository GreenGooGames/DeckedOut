// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentManager.generated.h"


UENUM(BlueprintType, meta = (Bitflags))
enum class EEquipmentSlot : uint8
{
	None = 0x00,
	LeftHand = 0x01,
	RightHand = 0x02,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DECKEDOUT_API UEquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentManager();

protected:
	virtual void BeginPlay() override;

public:
	// Map that identifies which Skeleton Socket corresponds to a slot.
	UPROPERTY(EditDefaultsOnly)
	TMap<EEquipmentSlot, FName> SocketMapping;

	bool Equip(TSoftObjectPtr<AActor>& Actor, const EEquipmentSlot RequestedSlot);
	bool Unequip(TSoftObjectPtr<AActor>& Actor);

	const TMap<EEquipmentSlot, TSoftObjectPtr<AActor>>& GetEquipment() const { return EquipedObjects; };

protected:
	TMap<EEquipmentSlot, TSoftObjectPtr<AActor>> EquipedObjects;

	EEquipmentSlot GetAvailableSlot(const EEquipmentSlot RequestedSlot) const;

};
