// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AITypes.h"

#include "TartarusSTTFindNavigableLocation.generated.h"

class UEnvQuery;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSTTFindNavigableLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UEnvQuery> EnvironmentQuery = nullptr;

	UPROPERTY(EditAnywhere, Category = "Output")
		FVector Location = FAISystem::InvalidLocation;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) override;

	bool RunLocationQuery(const UObject* const Owner, UEnvQuery* const LocationQuery);
	void OnLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);

private:
	void Reset();
};
