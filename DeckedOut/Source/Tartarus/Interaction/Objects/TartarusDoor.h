// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusDoor.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	None,
	Open,
	Closed,
	Blocked
};

DECLARE_EVENT_OneParam(ATartarusDoor, FStateChangedEvent, EDoorState /*DoorState*/);

UCLASS()
class TARTARUS_API ATartarusDoor : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusDoor();

protected:
	virtual void BeginPlay() override;

protected:
	// Visual representation of the door.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Collision on this object.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UBoxComponent> Collider = nullptr;

	/*
	* Changes the state of the door.
	* Return: True if the state is changed, false if the state remained the same.
	*/
	bool ChangeState(const EDoorState NewState);

	// Event fired when the state of the door changes.
	FStateChangedEvent& OnStateChanged() { return StateChangedEvent; };

	// Defines behavior to happen when the door state changes.
	UFUNCTION(BlueprintImplementableEvent)
		void BP_HandleStateChanged(EDoorState NewState);

private:
	FStateChangedEvent StateChangedEvent = FStateChangedEvent();
	EDoorState DoorState = EDoorState::Closed;

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
