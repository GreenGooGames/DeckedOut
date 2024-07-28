// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/CorrbolgInventoryEntryFragment.h"

#include "CorrbolgInventoryUIFragment.generated.h"

class UTexture2D;

/**
 * Inventory fragment defining data to display to the UI.
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryUIFragment : public UCorrbolgInventoryEntryFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Image = nullptr;
	
};
