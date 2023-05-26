// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TartarusItem.h"

#include "TartarusCard.generated.h"

class ATartarusTreasureHuntGameState;
class UTartarusGameModifier;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCard : public UTartarusItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Card")
		TMap<TObjectPtr<UTartarusGameModifier>, float> ModifierWeights;
};
