// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#include "TartarusDeleteContextAction.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusDeleteContextAction : public UTartarusContextAction
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(UTartarusContextActionListData* const ContextData) override;
};
