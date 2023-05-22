// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "TartarusSubInventoryData.generated.h"

class UTartarusContextAction;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSubInventoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	int32 GetNumberOfSlots() const { return NumberOfSlots; }
	FGameplayTag GetType() const { return InventoryId; }
	const FText& GetName() const { return Name; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Default")
		int32 NumberOfSlots = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
		FGameplayTag InventoryId = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FText Name = FText();
};
