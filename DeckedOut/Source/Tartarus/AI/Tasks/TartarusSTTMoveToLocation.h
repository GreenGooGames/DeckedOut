// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Navigation/PathFollowingComponent.h"

#include "TartarusSTTMoveToLocation.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSTTMoveToLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Input")
		FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Input")
		float AcceptanceRadius = 100.0f;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

private:
	TOptional<EPathFollowingResult::Type> PathFollowingResult;

	UFUNCTION()
	void OnPathFollowingCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
