// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class IInteractableInterface;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DECKEDOUT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractableRange = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractableAngle = 180.0f;

protected:
	UFUNCTION(BlueprintCallable)
	bool TryInteract(const FTransform& OriginTransform, const FVector& Offset = FVector::ZeroVector) const;
	bool GatherInteractableActors(const FVector& Origin, const float Radius, TArray<TObjectPtr<AActor>>& OutInteractableActors) const;

	IInteractableInterface* GetBestInteractableTarget(TArray<TObjectPtr<AActor>>& InteractableObjects, const FTransform& OriginTransform) const;
	float EvaluateInteractableActor(const TObjectPtr<AActor>& Interactable, const FTransform& OriginTransform) const;

};
