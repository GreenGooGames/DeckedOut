// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTags.h"

#include "CorrbolgInventoryViewWidget.generated.h"

class UCommonTileView;
class UCorrbolgInventoryManagerComponent;
class UCorrbolgInventorySettings;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Init(const UCorrbolgInventorySettings& Settings);

protected:
	/** TileView holding all display entries that represent the entry stored in the inventory. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTileView> TileView = nullptr;

	/** Updates the data and refreshes the tileview. */
	void Refresh() const;

private:
	/** The inventory filter to show items for. */
	FGameplayTag Filter = FGameplayTag::EmptyTag;

	/* Searches for an inventory component on the owning player. */
	UCorrbolgInventoryManagerComponent* FindInventoryComponent() const;

};
