// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/TartarusHelpers.h"

#include "TartarusInventorySlotWidgetData.generated.h"

class UTexture2D;

/**
 *
 */
UCLASS()
class TARTARUS_API UTartarusInventorySlotWidgetData : public UObject
{
	GENERATED_BODY()

public:
	void SetTexture(const TSoftObjectPtr<UTexture2D> SoftTexture) { Texture = SoftTexture; }
	TSoftObjectPtr<UTexture2D> GetTexture() const { return Texture; }

	void SetItemId(const FPrimaryAssetId& Id) { ItemId = Id; }
	FPrimaryAssetId GetItemId() const { return ItemId; }

	void SetStackSize(const int32 NumInStack) { StackSize = NumInStack; }
	int32 GetStackSize() const { return StackSize; }

protected:
	TSoftObjectPtr<UTexture2D> Texture = nullptr;
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;
	int32 StackSize = 0;
};
