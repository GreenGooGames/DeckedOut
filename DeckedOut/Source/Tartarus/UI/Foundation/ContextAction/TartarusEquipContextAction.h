// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#include "TartarusEquipContextAction.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusEquipContextAction : public UTartarusContextAction
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction() override;
};
