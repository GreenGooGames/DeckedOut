// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UObject/WeakInterfacePtr.h"

#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusWaitForInteractTask.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusWaitForInteractTask : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	virtual void Activate() override;

private:
	virtual void OnDestroy(bool AbilityEnded) override;

#pragma region Execution
public:
	/*
	* Retrieves the Current Target to interact with.
	* Return: The Interface WeakPtr of the best interactable target.
	*/
	TWeakInterfacePtr<ITartarusInteractableTargetInterface> GetTarget() { return CurrentTarget; }

protected:
	/** Starts the process of looking for nearby interactables and notifying listeners. */
	UFUNCTION()
	void GatherInteractables();

private:
	/** Handle to keep track of the timer responsible for executing the GatherInteractables method. */
	FTimerHandle GatherInteractablesTimerHandle = FTimerHandle();

	/** Interval in seconds to check for nearby interactables. */
	float GatherInteractiblesTimerInterval = 0.1f;

	/** The maximum range between the source and target for interaction to be possible. */
	float InteractableRange = 100.0f;

	/** The maximum angle between source and target for interaction to be possible. */
	float InteractableAngle = 180.0f;

	/** The current best target to interact with. */
	TWeakInterfacePtr<ITartarusInteractableTargetInterface> CurrentTarget = nullptr;

	/*
	* Performs a sphere trace to find all interactable targets.
	* Return: True, Interactable targets were found. False, no interactable targets are found.
	*/
	bool ScanForInteractables(const FVector& Origin, const float Radius, TArray<TObjectPtr<AActor>>& OutInteractableActors) const;

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
#pragma endregion
};
