// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "InventorySlotWidget.generated.h"

class UCommonLazyImage;
struct FItemData_Inventory;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UInventorySlotWidget : public UCommonActivatableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	void SetupDisplayData(const FItemData_Inventory& ItemData);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> DisplayImage = nullptr;
};
