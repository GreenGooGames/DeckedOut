// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "CorrbolgInventorySettings.generated.h"

/**
 * Settings to manipulate initialization of an inventory.
 */
UCLASS(Blueprintable)
class CORRBOLG_API UCorrbolgInventorySettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FGameplayTag& GetFilter() const { return Filter;}

protected:
	/** Filter to only allow entries of a defined types.*/
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Filter = FGameplayTag();
};
