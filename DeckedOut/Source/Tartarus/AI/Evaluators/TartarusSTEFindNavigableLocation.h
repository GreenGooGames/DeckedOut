// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"

#include "TartarusSTEFindNavigableLocation.generated.h"

class UEnvQuery;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSTEFindNavigableLocation : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UEnvQuery> EnvironmentQuery = nullptr;

	UPROPERTY(EditAnywhere, Category = "Output")
		FVector NavigableLocation = FVector::ZeroVector;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime);

	bool FindLocationQuery(UObject* Owner, UEnvQuery* PlacementQuery);
	void OnLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);

private:
	TOptional<EStateTreeRunStatus> Status;

	
};
