// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CorrbolgInventoryViewWidget.generated.h"

class UCommonTileView;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	/*
	* TileView holding all display entries that represent the entry stored in the inventory.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTileView> TileView = nullptr;
};
