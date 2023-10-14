// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "CorrbolgInventoryManagerComponent.generated.h"

class UCorrbolgInventory;
class UCorrbolgInventoryEntryDefinition;
class UCorrbolgInventorySettings;

/**
* Manages inventories and provides methods to manipulate them.
*/
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CORRBOLG_API UCorrbolgInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	* Stores an entry in an inventory.
	* @param EntryDefinition: The definition of the entry.
	* @return: True if the entry was stored, false if the entry could not be stored.
	*/
	bool StoreEntry(UCorrbolgInventoryEntryDefinition* const EntryDefinition) const;

	/**
	* Retrieves an entry from the inventory.
	* @param EntryId: The id of the entry to retrieve.
	* @return: The definition of the retrieved entry, nullptr if no entry was found.
	*/
	UCorrbolgInventoryEntryDefinition* RetrieveEntry(const FGuid& EntryId) const;

protected:
	/** List of invnetory settings to initialize inventories of.*/
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UCorrbolgInventorySettings>> InventorySettings = TArray<TSoftObjectPtr<UCorrbolgInventorySettings>>();

private:
	/** Collection holding all inventories managed by this component.*/
	UPROPERTY()
	TArray<TObjectPtr<UCorrbolgInventory>> Inventories = TArray<TObjectPtr<UCorrbolgInventory>>();

	/** Creates an inventory for each setting provided.*/
	void SetupInventories();

	UCorrbolgInventory* FindInventoryForFilter(const FGameplayTag& Filter) const;
};
