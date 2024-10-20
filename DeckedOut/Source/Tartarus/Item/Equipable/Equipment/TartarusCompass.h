// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable/TartarusEquipableItem.h"
#include "System/TartarusHelpers.h"

#include "TartarusCompass.generated.h"

class UArrowComponent;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusCompass : public ATartarusEquipableItem
{
	GENERATED_BODY()
	
public:
	ATartarusCompass();

protected:
	FVector TargetLocation = FTartarusHelpers::InvalidLocation;

#if WITH_EDITORONLY_DATA
public:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
#endif

#pragma region EquipableInterface
public:
	virtual void OnEquipped(AActor* const EquippedActor) override;
#pragma endregion
};
