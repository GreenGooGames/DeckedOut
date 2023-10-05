// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"

#include "CorrbolgInventoryEntryDefinition.generated.h"

class UCorrbolgInventoryEntryFragment;

/**
 * Defines all the data of an entry in the inventory.
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryEntryDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	* Initializes the entry its data.
	* @param Id: The id of the entry.
	* @param Name: Name of the entry.
	* @param Description: Description of the entry.
	* @param Type: The type filter of the entry.
	* @param Fragments: Fragment data of the entry.
	*/
	void Init(const FGuid& Id, const FText& Name, const FText& Description, const FGameplayTag& Type, const TArray<UCorrbolgInventoryEntryFragment*>& Fragments);

	/**
	* Initializes the entry by copying the definition.
	* @param EntryDefinition: Definition of the entry to store.
	*/
	void Init(const UCorrbolgInventoryEntryDefinition* const EntryDefinition);

	const FGuid& GetId() const { return EntryId; }
	const FText& GetName() const { return EntryName; }
	const FText& GetDescription() const { return EntryDescription; }
	const FGameplayTag& GetType() const { return EntryType; }
	const TArray<TObjectPtr<UCorrbolgInventoryEntryFragment>>& GetFragments() const { return EntryFragments; }

	/**
	* Checks if the Entry is stackable.
	* @return: True is the entry is stackable, false if the entry is not stackable.
	*/
	bool IsStackable() const;

private:
	/** The Id of the entry.*/
	FGuid EntryId = FGuid();

	/** The name of the entry.*/
	FText EntryName = FText();

	/** The description of the entry.*/
	FText EntryDescription = FText();

	/** Tag to identify what kind of entry this is.*/
	FGameplayTag EntryType = FGameplayTag();

	/** Container holding fragments that make up the unique definition.*/
	UPROPERTY()
	TArray<TObjectPtr<UCorrbolgInventoryEntryFragment>> EntryFragments = TArray<TObjectPtr<UCorrbolgInventoryEntryFragment>>();
};
