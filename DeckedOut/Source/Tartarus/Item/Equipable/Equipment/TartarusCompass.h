// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable/TartarusEquipableItem.h"
#include "System/TartarusHelpers.h"

#include "TartarusCompass.generated.h"

class UArrowComponent;

// TEMP
class ATartarusTreasureChest;
// END TEMP

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusCompass : public ATartarusEquipableItem
{
	GENERATED_BODY()
	
public:
	ATartarusCompass();

	void SetTargetLocation(const FVector& Location) { TargetLocation = Location; }

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
	virtual void OnEquipped() override;
#pragma endregion
};
