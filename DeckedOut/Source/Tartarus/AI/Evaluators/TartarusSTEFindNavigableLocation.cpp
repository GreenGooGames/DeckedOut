// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Evaluators/TartarusSTEFindNavigableLocation.h"

#include "StateTreeExecutionContext.h"
#include "AITypes.h"
#include "AI/TartarusAIController.h"
#include "Logging/TartarusLogChannels.h"

void UTartarusSTEFindNavigableLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!Status.IsSet())
    {
        if (FindLocationQuery(Context.GetOwner(), EnvironmentQuery))
        {
            Status = EStateTreeRunStatus::Running;
            return;
        }
    }
}

bool UTartarusSTEFindNavigableLocation::FindLocationQuery(UObject* Owner, UEnvQuery* PlacementQuery)
{
    if (PlacementQuery)
    {
        APawn* const OwnerPawn = Cast<APawn>(Owner);
        if (!IsValid(OwnerPawn))
        {
            UE_LOG(LogTartarus, Log, TEXT("%s: Failed to start Find Location Query: Owner is not a pawn!"), *FString(__FUNCTION__));
            return false;
        }

        // Set up a query request
        FEnvQueryRequest QueryRequest(PlacementQuery, OwnerPawn->GetController());
        QueryRequest.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &UTartarusSTEFindNavigableLocation::OnLocationQueryFinished);

        return true;
    }

    return false;
}

void UTartarusSTEFindNavigableLocation::OnLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (Result.IsValid() && Result->IsSuccessful())
    {
        NavigableLocation = Result->GetItemAsLocation(0);
    }
    else
    {
        NavigableLocation = FAISystem::InvalidLocation;
    }

    Status.Reset();
}
