// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusActionBarWidget.generated.h"

class UCommonBoundActionBar;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusActionBarWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (bindwidget))
		TObjectPtr<UCommonBoundActionBar> BoundActionBar = nullptr;
};
