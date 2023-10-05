// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"

#include "CorrbolgInventoryTesting.generated.h"

class UCorrbolgInventoryManagerComponent;

/**
 * Provides utility to test the Corrbolg inventory.
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryTesting : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintcallable)
	static bool AddItemToInventory(UCorrbolgInventoryManagerComponent* const Inventory, const FGameplayTag& ItemFilter, const FGuid& ItemId);

	UFUNCTION(Blueprintcallable)
	static bool RemoveItemFromInventory(UCorrbolgInventoryManagerComponent* const Inventory, const FGuid& ItemId);
};
