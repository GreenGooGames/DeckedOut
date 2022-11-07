// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusItemBase.generated.h"

UCLASS()
class TARTARUS_API ATartarusItemBase : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusItemBase();

	void SetReferenceId(const int32 ItemReferenceId) { ReferenceId = ItemReferenceId; }

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

private:
	int32 ReferenceId = -1;

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
