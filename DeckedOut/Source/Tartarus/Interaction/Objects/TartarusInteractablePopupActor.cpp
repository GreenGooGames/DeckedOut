// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Objects/TartarusInteractablePopupActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"
#include "UI/Gameplay/TartarusInteractionWidget.h"
#include "UI/Foundation/TartarusWidgetComponent.h"

// Sets default values
ATartarusInteractablePopupActor::ATartarusInteractablePopupActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collider
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetSimulatePhysics(false);
	Collider->SetCollisionProfileName(FName("Trigger"), false);

	SetRootComponent(Collider);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetupAttachment(Collider);

	// Interaction
	CreateInteractionWidgetComponent();
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusInteractablePopupActor::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusInteractablePopupActor::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// Show the UI linked to this actor.
	UTartarusPrimaryGameLayout* const PrimaryGameLayout = UTartarusPrimaryGameLayout::Get(InstigatorController.Get());
	if (!IsValid(PrimaryGameLayout))
	{
		return false;
	}

	PrimaryGameLayout->PushWidgetToLayerAsync(WidgetLayer, WidgetTemplate);

	return true;
}

void ATartarusInteractablePopupActor::DisableInteraction()
{
	check(false);
	UE_LOG(LogTartarus, Warning, TEXT("%s: Not Implemented!"), *FString(__FUNCTION__));
}

bool ATartarusInteractablePopupActor::ToggleInteractionPrompt(const bool bShowPrompt)
{
	if (!IsValid(InteractionWidgetComponent))
	{
		return false;
	}

	InteractionWidgetComponent->SetHiddenInGame(!bShowPrompt);

	return true;
}

void ATartarusInteractablePopupActor::CreateInteractionWidgetComponent()
{
	// Create and setup the Widget Component to display the interaction prompt.
	InteractionWidgetComponent = CreateDefaultSubobject<UTartarusWidgetComponent>(TEXT("InteractionWidget"));
	if (!IsValid(InteractionWidgetComponent))
	{
		return;
	}

	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidgetComponent->SetSimulatePhysics(false);
	InteractionWidgetComponent->SetHiddenInGame(true);

	// Setup the Widget to display with the correct data.
	InteractionWidgetComponent->OnWidgetCreatedEvent().AddLambda([&](UTartarusWidgetComponent* WidgetComponent)
		{
			// Setup the Widget to display with the correct data.
			UTartarusInteractionWidget* const InteractionWidget = Cast<UTartarusInteractionWidget>(WidgetComponent->GetWidget());
			if (!IsValid(InteractionWidget))
			{
				return;
			}

			InteractionWidget->SetText(InteractionText);
			InteractionWidget->UpdateInputActionWidget();
		});
}
#pragma endregion
