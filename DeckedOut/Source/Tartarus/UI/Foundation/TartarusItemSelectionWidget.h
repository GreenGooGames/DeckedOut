// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusItemSelectionWidget.generated.h"

class UCommonTileView;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemSelectionWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTileView> TileView = nullptr;
};
