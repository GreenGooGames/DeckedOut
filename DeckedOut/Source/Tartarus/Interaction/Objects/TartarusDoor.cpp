// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Objects/TartarusDoor.h"

#include "Components/BoxComponent.h"

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
	return true;
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
	case EDoorState::Blocked:
	default:
		break;
	}

	if (bHasStateChanged)
	{
		HandleStateChanged(DoorState, InstigatorController);
	}

	return bHasStateChanged;
}
#pragma endregion
