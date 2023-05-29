// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "GameplayTagContainer.h"

#include "TartarusInteractablePopupActor.generated.h"

class UBoxComponent;
class UTartarusLayoutWidget;
class UTartarusWidgetComponent;

UCLASS()
class TARTARUS_API ATartarusInteractablePopupActor : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusInteractablePopupActor();

protected:
	// Visual representation of this actor. (No collision)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Collision on this object.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UBoxComponent> Collider = nullptr;

private:
	// The layer to push the widget to on interaction.
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
		FGameplayTag WidgetLayer = FGameplayTag::EmptyTag;

	// The template of the widget to show on interaction.
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
		TSoftClassPtr<UTartarusLayoutWidget> WidgetTemplate = nullptr;

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
};
