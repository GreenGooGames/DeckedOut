// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "CorrbolgInventoryViewListItem.generated.h"

class UTexture2D;

struct FCorrbolgInventoryEntry;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewListItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FCorrbolgInventoryEntry& Entry);

	const TSoftObjectPtr<UTexture2D>& GetTexture() const {return Texture;}

	const FCorrbolgInventoryEntry* const GetInventoryEntry() const {return InventoryEntry;}

private:
	/** The image to display for this item. */
	TSoftObjectPtr<UTexture2D> Texture = nullptr;

	const FCorrbolgInventoryEntry* InventoryEntry = nullptr;
};
