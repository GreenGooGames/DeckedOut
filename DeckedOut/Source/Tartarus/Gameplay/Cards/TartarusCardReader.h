// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusCardReader.generated.h"

class UBoxComponent;

UCLASS()
class TARTARUS_API ATartarusCardReader : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusCardReader();

protected:
	// Visual representation of the door.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Collision on this object.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UBoxComponent> Collider = nullptr;

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
