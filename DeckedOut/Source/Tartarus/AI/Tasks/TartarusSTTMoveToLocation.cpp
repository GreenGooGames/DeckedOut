// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/TartarusSTTMoveToLocation.h"

#include "StateTreeExecutionContext.h"
#include "AI/TartarusAIController.h"
#include "Logging/TartarusLogChannels.h"

void UTartarusSTTMoveToLocation::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);

	Reset();
}

EStateTreeRunStatus UTartarusSTTMoveToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// Wait for a valid location.
	if (Location != FAISystem::InvalidLocation && !PathFollowingResult.IsSet())
	{
		if (RequestMoveTo(Context))
		{
			PathFollowingResult = EPathFollowingResult::Invalid;
		}
	}

	if (PathFollowingResult.IsSet() && PathFollowingResult != EPathFollowingResult::Invalid)
	{
		return PathFollowingResult == EPathFollowingResult::Success ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

bool UTartarusSTTMoveToLocation::RequestMoveTo(FStateTreeExecutionContext& Context) const
{
	APawn* const OwnerPawn = Cast<APawn>(Context.GetOwner());
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Request Move To: Owner is not a pawn!"), *FString(__FUNCTION__));
		return false;
	}

	ATartarusAIController* const Controller = Cast<ATartarusAIController>(OwnerPawn->GetController());
	if (!IsValid(Controller))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Request Move To: Owner does not have an AI Controller!"), *FString(__FUNCTION__));
		return false;
	}

	const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(Location, AcceptanceRadius);
	if (Result == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Request Move To: Path Following Request failed!"), *FString(__FUNCTION__));
		return false;
	}

	Controller->ReceiveMoveCompleted.AddUniqueDynamic(this, &UTartarusSTTMoveToLocation::OnPathFollowingCompleted);
	return true;
}

void UTartarusSTTMoveToLocation::OnPathFollowingCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	PathFollowingResult = Result;
}

void UTartarusSTTMoveToLocation::Reset()
{
	PathFollowingResult.Reset();
}
