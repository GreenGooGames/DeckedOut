// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Objects/TartarusDoor.h"

#include "TartarusTreasureHuntEntrance.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusTreasureHuntEntrance : public ATartarusDoor
{
	GENERATED_BODY()
	
public:
	virtual void PostInitializeComponents() override;

protected:
	virtual bool CanChangeState(const EDoorState NewState) const;

	// Executed when the player begins overlaps with the trigger.
	UFUNCTION()
		void OnColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Executed when the player ends overlaps with the trigger.
	UFUNCTION()
		void OnColliderEndOverlap(	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
#pragma endregion
};
