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
	ATartarusTreasureHuntEntrance();

protected:
	// Trigger to determine the player is on the other side of the door.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UBoxComponent> CloseTrigger = nullptr;

	virtual bool CanChangeState(const EDoorState NewState) const;

	// Executed when the player overlaps with the close trigger.
	UFUNCTION()
		void OnCloseTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
#pragma endregion
};
