// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TartarusCharacter.h"

#include "TartarusAICharacter.generated.h"

class UStateTreeComponent;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusAICharacter : public ATartarusCharacter
{
	GENERATED_BODY()
	
public:
	ATartarusAICharacter();

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UStateTreeComponent> StateTreeComponent = nullptr;
};
