// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ItemData.h"
#include "DeckedOut/Components/Interaction/InteractableInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

UCLASS()
class DECKEDOUT_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	// The physics volume that bounds the item.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Volume = nullptr;

	FItemData ItemData = FItemData();
	
#pragma region IInteractableInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
