// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusContextActionBulk.generated.h"

class UTartarusContextAction;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextActionBulk : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const TArray<UTartarusContextAction*>& GetContextActions() { return ContextActions; }

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
		TArray<UTartarusContextAction*> ContextActions;
};
