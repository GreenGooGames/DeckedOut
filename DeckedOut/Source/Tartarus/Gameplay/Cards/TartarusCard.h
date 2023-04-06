// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusCard.generated.h"

class ATartarusTreasureHuntGameState;
enum class EGameModifier : uint8;
/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCard : public UDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
		TMap<EGameModifier, float> Modifiers;

protected:
	void ApplyModifiers(ATartarusTreasureHuntGameState* const GameState) const;
	void RemoveModifiers(ATartarusTreasureHuntGameState* const GameState) const;
};
