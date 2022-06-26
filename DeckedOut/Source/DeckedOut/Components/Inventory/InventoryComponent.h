// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DeckedOut/World/Items/ItemData.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// [Koen Goossens] TODO: Add a stacksize limit.
struct FStoredItemData
{
	FItemData ItemData = FItemData();
	int32 StackSize = -1;

	FStoredItemData(const FItemData& InItemData, const int32 InStackSize);
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DECKEDOUT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
	virtual bool RetrieveItems(const int32 ItemId, const int32 Amount, FItemData& OutItemData, int32& OutRetrievedAmount);
	UFUNCTION(BlueprintCallable)
	virtual bool StoreItem(const FItemData& ItemData, const int32 Amount);

protected:
	// [Koen Goossens] TODO: Add inventory space limit.
	TArray<FStoredItemData> StoredItems;

	int32 FindIndexOfStoredItemData(const int32 ItemId) const;
};
