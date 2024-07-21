// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CorrbolgInventoryViewListItem.generated.h"

class UTexture2D;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewListItem : public UObject
{
	GENERATED_BODY()

public:
	void Init();

private:
	/** The image to display for this item. */
	TSoftObjectPtr<UTexture2D> Texture = nullptr;

	/** The stacksize for this item. */
	int32 StackSize = 0;
};
