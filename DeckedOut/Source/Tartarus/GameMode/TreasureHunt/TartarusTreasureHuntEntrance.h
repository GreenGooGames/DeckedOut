// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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

	// Item to be gifted when the player opens the door.
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle GiftItemRow;

	// Executed when the state of the door is changed.
	virtual void HandleStateChanged(const EDoorState NewState, AController* const InstigatorController) override;

	// Executed when the player overlaps with the close trigger.
	UFUNCTION()
		void OnCloseTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
#pragma endregion
};
