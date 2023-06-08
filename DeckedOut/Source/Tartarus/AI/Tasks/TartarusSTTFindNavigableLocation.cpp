// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/TartarusSTTFindNavigableLocation.h"

#include "StateTreeExecutionContext.h"
#include "AI/TartarusAIController.h"
#include "Logging/TartarusLogChannels.h"

EStateTreeRunStatus UTartarusSTTFindNavigableLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    Super::EnterState(Context, Transition);

    if (RunLocationQuery(Context.GetOwner(), EnvironmentQuery))
    {
       return EStateTreeRunStatus::Running;
    }

    return EStateTreeRunStatus::Failed;
}

void UTartarusSTTFindNavigableLocation::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
    Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);

    Reset();
}

bool UTartarusSTTFindNavigableLocation::RunLocationQuery(const UObject* const Owner, UEnvQuery* const LocationQuery)
{
    if (LocationQuery)
    {
        const APawn* const OwnerPawn = Cast<APawn>(Owner);
        if (!IsValid(OwnerPawn))
        {
            UE_LOG(LogTartarus, Log, TEXT("%s: Failed to start Find Location Query: Owner is not a pawn!"), *FString(__FUNCTION__));
            return false;
        }

        // Set up a query request
        FEnvQueryRequest QueryRequest(LocationQuery, OwnerPawn->GetController());
        QueryRequest.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &UTartarusSTTFindNavigableLocation::OnLocationQueryFinished);

        return true;
    }

    return false;
}

void UTartarusSTTFindNavigableLocation::OnLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (Result.IsValid() && Result->IsSuccessful())
    {
        Location = Result->GetItemAsLocation(0);
    }
    else
    {
        Location = FAISystem::InvalidLocation;
    }
}

void UTartarusSTTFindNavigableLocation::Reset()
{
    Location = FAISystem::InvalidLocation;
}
