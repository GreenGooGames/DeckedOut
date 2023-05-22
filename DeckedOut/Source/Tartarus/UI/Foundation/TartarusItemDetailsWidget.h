// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusItemDetailsWidget.generated.h"

class UCommonLazyImage;
class UCommonListView;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemDetailsWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> ItemImage = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonListView> ItemInfoListView = nullptr;
};
