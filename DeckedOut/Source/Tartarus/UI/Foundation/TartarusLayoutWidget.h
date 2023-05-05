// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "TartarusLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusLayoutWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetOwningLayerName(const FGameplayTag& LayerName);
	FORCEINLINE FGameplayTag& GetOwningLayerName();

protected:
	/* Name of the layer to add this widget to when pushed to the primary game layout */
	FGameplayTag OwningLayerName = FGameplayTag();
};
