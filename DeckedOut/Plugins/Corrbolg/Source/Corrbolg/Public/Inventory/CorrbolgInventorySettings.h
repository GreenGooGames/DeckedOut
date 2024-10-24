// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "CorrbolgInventorySettings.generated.h"

class UCorrbolgContextActionDefinition;

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
	const TArray<TSoftObjectPtr<UCorrbolgContextActionDefinition>>& GetContextActions() const {return ContextActions;}

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

	/** The context actions available on entries of the inventory, used to display context options in the UI.*/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSoftObjectPtr<UCorrbolgContextActionDefinition>> ContextActions = TArray<TSoftObjectPtr<UCorrbolgContextActionDefinition>>();
};
