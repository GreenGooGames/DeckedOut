// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusContextAction.generated.h"

class UTartarusContextActionListData;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextAction : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(UTartarusContextActionListData* const ContextData) { return; };
	const FText& GetName() const { return Name; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FText Name = FText();
};
