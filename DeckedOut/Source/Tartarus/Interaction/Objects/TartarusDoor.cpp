// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Objects/TartarusDoor.h"

#include "Audio/TartarusNoiseSourceComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "TartarusDoor.h"
#include "UI/Gameplay/TartarusInteractionWidget.h"
#include "UI/Foundation/TartarusWidgetComponent.h"

// Sets default values
ATartarusDoor::ATartarusDoor()
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

	// Audio
	NoiseSourceComponent = CreateDefaultSubobject<UTartarusNoiseSourceComponent>(TEXT("Noise Source Component"));

	// Interaction
	CreateInteractionWidgetComponent();
}

void ATartarusDoor::BeginPlay()
{
	OnStateChanged().AddUObject(this, &ATartarusDoor::BP_HandleStateChanged);
}

bool ATartarusDoor::ChangeState(const EDoorState NewState)
{
	const bool bCanStateBeChanged = CanChangeState(NewState);
	if (!bCanStateBeChanged)
	{
		return false;
	}

	DoorState = NewState;

	return true;
}

bool ATartarusDoor::CanChangeState(const EDoorState NewState) const
{
	// Do not attempt to change the state if the new state is the same as the current state.
	const bool bCanChangeState = DoorState != NewState;

	return bCanChangeState;
}

void ATartarusDoor::HandleStateChanged(const EDoorState NewState, AController* const InstigatorController)
{
	OnStateChanged().Broadcast(DoorState);
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusDoor::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusDoor::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	bool bHasStateChanged = false;

	// Try to change the current state of the door. (Open --> Closed, Closed --> Open, Blocked --> Open)
	switch (DoorState)
	{
	case EDoorState::Blocked:
	{
		GenerateNoise();
		break;
	}
	case EDoorState::Open:
	{
		bHasStateChanged = ChangeState(EDoorState::Closed);
		break;
	}
	case EDoorState::Closed:
	{
		bHasStateChanged = ChangeState(EDoorState::Open);
		break;
	}
	// Intentional falltrough, NOT IMPLEMENTED
	case EDoorState::None:
	default:
		break;
	}

	if (bHasStateChanged)
	{
		HandleStateChanged(DoorState, InstigatorController);
		GenerateNoise();
	}

	return bHasStateChanged;
}

void ATartarusDoor::DisableInteraction()
{
	check(false);
	UE_LOG(LogTartarus, Warning, TEXT("%s: Not Implemented!"), *FString(__FUNCTION__));
}

bool ATartarusDoor::ToggleInteractionPrompt(const bool bShowPrompt)
{
	if (!IsValid(InteractionWidgetComponent))
	{
		return false;
	}

	InteractionWidgetComponent->SetHiddenInGame(!bShowPrompt);

	return true;
}

void ATartarusDoor::CreateInteractionWidgetComponent()
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

#pragma region Audio
void ATartarusDoor::GenerateNoise() const
{
	switch (DoorState)
	{
	case EDoorState::Open:
		NoiseSourceComponent->GenerateNoise(OpeningSound, GetActorLocation());
		break;
	case EDoorState::Closed:
		NoiseSourceComponent->GenerateNoise(ClosingSound, GetActorLocation());
		break;
	case EDoorState::Blocked:
		NoiseSourceComponent->GenerateNoise(BlockedSound, GetActorLocation());
		break;
	case EDoorState::None:
	default:
		break;
	}
}
#pragma endregion
