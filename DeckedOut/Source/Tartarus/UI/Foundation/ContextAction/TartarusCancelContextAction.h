// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#include "TartarusCancelContextAction.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCancelContextAction : public UTartarusContextAction
{
	GENERATED_BODY()
	
public:
	// Close the ContextMenu this action belongs to.
	virtual void ExecuteAction(UTartarusContextActionListData* const ContextData) override;
};
