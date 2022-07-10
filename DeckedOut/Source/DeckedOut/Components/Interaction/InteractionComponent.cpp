// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "InteractableInterface.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UInteractionComponent::TryInteract(const FTransform& OriginTransform, const FVector& Offset) const
{
	TArray<TObjectPtr<AActor>> InteractableActors;
	
	if (GatherInteractableActors(OriginTransform.GetLocation() + Offset, InteractableRange, InteractableActors))
	{
		// [Koen Goossens] Have UI popup that list all objects. (Genshin Impact)
		// For now get a single best target and immediately interact.
		if (IInteractableInterface* const BestTarget = GetBestInteractableTarget(InteractableActors, OriginTransform))
		{
			return BestTarget->StartInteraction(Cast<AController>(GetOwner()));
		}
	}

	return false;
}

bool UInteractionComponent::GatherInteractableActors(const FVector& Origin, const float Radius, TArray<TObjectPtr<AActor>>& OutInteractableActors) const
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
		const IInteractableInterface* const Interactable = Cast<IInteractableInterface>(Hit.GetActor());

		if (Interactable && Interactable->IsInteractable())
		{
			OutInteractableActors.Add(Hit.GetActor());
		}
	}

	return OutInteractableActors.Num() > 0;
}

IInteractableInterface* UInteractionComponent::GetBestInteractableTarget(TArray<TObjectPtr<AActor>>& InteractableActors, const FTransform& OriginTransform) const
{
	TObjectPtr<AActor> BestTarget = nullptr;
	float BestScore = -1.0f;

	for (TObjectPtr<AActor>& Interactable : InteractableActors)
	{
		const float Score = EvaluateInteractableActor(Interactable, OriginTransform);

		if (Score > BestScore)
		{
			BestTarget = Interactable;
		}
	}

	return Cast<IInteractableInterface>(BestTarget);
}

float UInteractionComponent::EvaluateInteractableActor(const TObjectPtr<AActor>& Interactable, const FTransform& OriginTransform) const
{
	if (!Interactable.IsNull())
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