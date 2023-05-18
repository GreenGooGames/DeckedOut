// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TartarusInteractableSourceComponent.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TartarusInteractableSourceComponent.h"
#include "Player/TartarusPlayerController.h"
#include "GameFramework/Pawn.h"

UTartarusInteractableSourceComponent::UTartarusInteractableSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTartarusInteractableSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<TObjectPtr<AActor>> InteractableActors;
	ATartarusPlayerController* Owner = Cast<ATartarusPlayerController>(GetOwner());
	const FVector Location = Owner->GetPawn()->GetActorLocation();

	if (GatherInteractableActors(Location, InteractableRange, InteractableActors))
	{
		ITartarusInteractableTargetInterface* const BestTarget = GetBestInteractableTarget(InteractableActors, GetOwner()->GetTransform());
		if (CurrentTarget.Get() == BestTarget)
		{
			return;
		}

		if (CurrentTarget.IsValid())
		{
			CurrentTarget->ToggleInteractionPrompt(false);
		}

		if (BestTarget)
		{
			BestTarget->ToggleInteractionPrompt(true);
		}

		CurrentTarget = BestTarget;
	}
	else
	{
		if (CurrentTarget.IsValid())
		{
			CurrentTarget->ToggleInteractionPrompt(false);
			CurrentTarget = nullptr;
		}
	}
}

bool UTartarusInteractableSourceComponent::TryInteract(const FTransform& OriginTransform, const FVector& Offset) const
{
	if (CurrentTarget.Get())
	{
		const bool bSuccess = CurrentTarget->StartInteraction(Cast<AController>(GetOwner()));
		return bSuccess;
	}

	return false;
}

bool UTartarusInteractableSourceComponent::GatherInteractableActors(const FVector& Origin, const float Radius, TArray<TObjectPtr<AActor>>& OutInteractableActors) const
{
	OutInteractableActors.Empty();

	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutHits;

	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Origin, Origin, Radius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1), false,
		ActorsToIgnore, EDrawDebugTrace::None, OutHits, true);

	for (const FHitResult& Hit : OutHits)
	{
		const ITartarusInteractableTargetInterface* const Interactable = Cast<ITartarusInteractableTargetInterface>(Hit.GetActor());

		if (Interactable && Interactable->IsInteractable())
		{
			OutInteractableActors.Add(Hit.GetActor());
		}
	}

	return OutInteractableActors.Num() > 0;
}

ITartarusInteractableTargetInterface* UTartarusInteractableSourceComponent::GetBestInteractableTarget(TArray<TObjectPtr<AActor>>& InteractableObjects, const FTransform& OriginTransform) const
{
	TObjectPtr<AActor> BestTarget = nullptr;
	float BestScore = -1.0f;

	for (TObjectPtr<AActor>& Interactable : InteractableObjects)
	{
		const float Score = EvaluateInteractableActor(Interactable, OriginTransform);

		if (Score > BestScore)
		{
			BestTarget = Interactable;
		}
	}

	return Cast<ITartarusInteractableTargetInterface>(BestTarget);
}

float UTartarusInteractableSourceComponent::EvaluateInteractableActor(const TObjectPtr<AActor>& Interactable, const FTransform& OriginTransform) const
{
	if (Interactable)
	{
		const float MaxAngle = FMath::DegreesToRadians(InteractableAngle);

		const FVector OriginToActor = FVector(OriginTransform.GetLocation() - Interactable->GetActorLocation()).GetSafeNormal();
		const FVector OriginDirection = OriginTransform.GetRotation().Vector();

		const float Dot = FVector::DotProduct(OriginToActor, OriginDirection);
		const float ACos = FMath::Acos(Dot);

		if (ACos <= MaxAngle)
		{
			// The closer the angle is to 0 the higher the score scaled to 1.0f;
			const float Score = 1 - (ACos / MaxAngle);

			return Score;
		}
	}

	return -1.0f;
}
