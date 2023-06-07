// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/TartarusSTTMoveToLocation.h"

#include "StateTreeExecutionContext.h"
#include "AI/TartarusAIController.h"
#include "Logging/TartarusLogChannels.h"

EStateTreeRunStatus UTartarusSTTMoveToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	APawn* const OwnerPawn = Cast<APawn>(Context.GetOwner());
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to enter state: Owner is not a pawn!"), *FString(__FUNCTION__));
		return EStateTreeRunStatus::Failed;
	}

	ATartarusAIController* const Controller = Cast<ATartarusAIController>(OwnerPawn->GetController());
	if (!IsValid(Controller))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to enter state: Owner does not have an AI Controller!"), *FString(__FUNCTION__));
		return EStateTreeRunStatus::Failed;
	}

	const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(Location, AcceptanceRadius);
	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}

	Controller->ReceiveMoveCompleted.AddUniqueDynamic(this, &UTartarusSTTMoveToLocation::OnPathFollowingCompleted);
	PathFollowingResult.Reset();

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UTartarusSTTMoveToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (PathFollowingResult.IsSet())
	{
		return PathFollowingResult.GetValue() == EPathFollowingResult::Success ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

void UTartarusSTTMoveToLocation::OnPathFollowingCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	PathFollowingResult = Result;
}
