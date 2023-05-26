// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "TartarusGameModifierEntryData.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameModifierEntryData : public UObject
{
	GENERATED_BODY()
	
public:
	FText ModifierName = FText();
	FText ModifierTooltip = FText();
	float ModifierWeight = 0.0f;
};
