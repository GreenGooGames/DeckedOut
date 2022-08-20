// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeckedOut/World/Items/EquipableInterface.h"
#include "DeckedOut/World/Items/Item.h"

#include "Compass.generated.h"

const FString TargetLocationDataName = "TargetLocation";

class UArrowComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class DECKEDOUT_API ACompass : public AItem, public IEquipableInterface
{
	GENERATED_BODY()

public:
	ACompass();
	virtual void Tick(float DeltaSeconds) override;

	void SetTargetLocation(const FVector& Location) { TargetLocation = Location; }
	virtual bool WriteUniqueData(const TMap<FString, ItemUniqueDataType>& UniqueData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	FVector TargetLocation = FVector::ZeroVector;

	virtual TMap<FString, ItemUniqueDataType> GenerateUniqueData() override;

#pragma region IEquipableInterface
public:
	virtual void OnEquip() override;
	virtual void OnUnequip() override;

#pragma endregion
};
