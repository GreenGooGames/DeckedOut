// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TartarusItem.h"

#include "TartarusCard.generated.h"

class ATartarusTreasureHuntGameState;
enum class EGameModifier : uint8;
/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCard : public UTartarusItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Card")
		TMap<EGameModifier, float> Modifiers;
};
