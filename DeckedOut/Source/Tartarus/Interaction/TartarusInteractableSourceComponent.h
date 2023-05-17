// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/WeakInterfacePtr.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusInteractableSourceComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusInteractableSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTartarusInteractableSourceComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Starts interaction with the best target that satisfies certain conditions.
	* Return: True, started interaction. False, failed to start interaction.
	*/
	bool TryInteract(const FTransform& OriginTransform, const FVector& Offset = FVector::ZeroVector) const;

protected:
	// The maximum range between the source and target for interaction to be possible.
	UPROPERTY(EditDefaultsOnly)
		float InteractableRange = 100.0f;

	// The maximum angle between source and target for interaction to be possible.
	UPROPERTY(EditDefaultsOnly)
		float InteractableAngle = 180.0f;

	TWeakInterfacePtr<ITartarusInteractableTargetInterface> CurrentTarget = nullptr;

	/*
	* Performs a sphere trace to find all interactable targets.
	* Return: True, Interactable targets were found. False, no interactable targets are found.
	*/
	bool GatherInteractableActors(const FVector& Origin, const float Radius, TArray<TObjectPtr<AActor>>& OutInteractableActors) const;

	/*
	* Evaluates every interactable target to see which one is the best target.
	* Return: The best interactable target.
	*/
	ITartarusInteractableTargetInterface* GetBestInteractableTarget(TArray<TObjectPtr<AActor>>& InteractableObjects, const FTransform& OriginTransform) const;

	/*
	* Evaluates the actor based on conditions.
	* Return: A score that defines the liklyhood of this being the best target.
	*/
	float EvaluateInteractableActor(const TObjectPtr<AActor>& Interactable, const FTransform& OriginTransform) const;

};
