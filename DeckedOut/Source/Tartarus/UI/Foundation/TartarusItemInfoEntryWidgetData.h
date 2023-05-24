// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TartarusItemInfoEntryWidgetData.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemInfoEntryWidgetData : public UObject
{
	GENERATED_BODY()
	
public:
	FText MainText = FText();
	FText SubText = FText();
};
