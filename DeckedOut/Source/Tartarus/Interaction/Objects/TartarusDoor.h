// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Audio/TartarusSoundData.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusDoor.generated.h"

class UBoxComponent;
class UTartarusNoiseSourceComponent;
class UTartarusWidgetComponent;

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

	/*
	* Performs checks to see if the state can be changed.
	* Return: True if all checks passed. False if something prohibits changing the state.
	*/
	virtual bool CanChangeState(const EDoorState NewState) const;

	// Event fired when the state of the door changes.
	FStateChangedEvent& OnStateChanged() { return StateChangedEvent; };

	virtual void HandleStateChanged(const EDoorState NewState, AController* const InstigatorController);

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
	virtual void DisableInteraction() override;
	virtual bool ToggleInteractionPrompt(const bool bShowPrompt);

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusWidgetComponent> InteractionWidgetComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		FText InteractionText = FText();

	void CreateInteractionWidgetComponent();
#pragma endregion

#pragma region Audio
protected:
	// Sound to play when opening.
	UPROPERTY(EditDefaultsOnly)
		FTartarusSound OpeningSound = FTartarusSound();

	// Sound to play when closing.
	UPROPERTY(EditDefaultsOnly)
		FTartarusSound ClosingSound = FTartarusSound();

	// Sound to play when trying to open/close but blocked.
	UPROPERTY(EditDefaultsOnly)
		FTartarusSound BlockedSound = FTartarusSound();

	// Component that handles generating noise/sounds.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusNoiseSourceComponent> NoiseSourceComponent = nullptr;

	virtual void GenerateNoise() const;
#pragma endregion
};
