// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "System/TartarusHelpers.h"

#include "TartarusInventorySlotWidgetData.generated.h"

class UTexture2D;

DECLARE_EVENT_OneParam(UTartarusInventorySlotWidgetData, FOnListItemDataChanged, const UObject*);

/**
 *
 */
UCLASS()
class TARTARUS_API UTartarusInventorySlotWidgetData : public UObject
{
	GENERATED_BODY()

public:
	void SetTexture(TSoftObjectPtr<UTexture2D> DisplayTexture);
	TSoftObjectPtr<UTexture2D> GetTexture() const { return Texture; }

	void UpdateData(const FInventoryStack* const InventoryStack);

	FPrimaryAssetId GetItemId() const { return ItemId; }
	const FInventoryStackId& GetInventoryStackId() const { return InventoryStackId; }
	int32 GetStackSize() const { return StackSize; }

	FOnListItemDataChanged& GetOnlistItemDataChangedEvent() { return OnListItemDataChangedEvent; }

private:
	TSoftObjectPtr<UTexture2D> Texture = nullptr;
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;
	FInventoryStackId InventoryStackId = FInventoryStackId();
	int32 StackSize = 0;

	FOnListItemDataChanged OnListItemDataChangedEvent = FOnListItemDataChanged();
};
