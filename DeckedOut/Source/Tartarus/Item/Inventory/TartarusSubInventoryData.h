// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/Inventory/TartarusInventoryData.h"

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
	EInventoryType GetType() const { return InventoryType; }
	const FText& GetName() const { return Name; }
	TArray<TObjectPtr<UTartarusContextAction>> GetContextActions() const { return ContextActions; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Default")
		int32 NumberOfSlots = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
		EInventoryType InventoryType = EInventoryType::MAX;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FText Name = FText();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TArray<TObjectPtr<UTartarusContextAction>> ContextActions;
};