// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "UI/Inventory/CorrbolgInventoryViewWidget.h"

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
	const FText& GetDisplayName() const { return DisplayName;}
	const int& GetEntryLimit() const {return EntryLimit;}

protected:
	/** Filter to only allow entries of a defined types.*/
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FGameplayTag Filter = FGameplayTag::EmptyTag;

	/** Number of individual entries to hold, negative value indicates unlimited.*/
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	int EntryLimit = -1;

	/** Name of the inventory, used to identify in UI.*/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText DisplayName = FText();
};
