// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DeckedOut/World/Items/ItemData.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DECKEDOUT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;

	virtual bool RetrieveItems(const int32 ItemId, const int32 Amount, int32& OutRetrievedAmount, TMap<FString, ItemUniqueDataType>& OutUniqueData);
	virtual bool StoreItem(const FItemData& ItemData, const TMap<FString, ItemUniqueDataType>& UniqueData, const int32 Amount);

	int32 GetInventorySize() const { return NumInventorySlots; }
	const TArray<FItemData_Inventory>& GetStoredItems() const;

protected:
	// The maximum amount of slots in the inventory.
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 NumInventorySlots = 0;
	TArray<FItemData_Inventory> StoredItems;

	// [Koen Goossens] TODO: Add an instanced Actor Inventory:
	// TArray<TObjectPtr<AActor>> InstancedInventory;

	int32 FindIndexOfStoredItemData(const int32 ItemId) const;
	int32 FindFirstAvailableSlot() const;
};
