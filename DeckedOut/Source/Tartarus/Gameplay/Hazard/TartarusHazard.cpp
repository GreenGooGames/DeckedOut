// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Hazard/TartarusHazard.h"

#include "Gameplay/Hazard/TartarusHazardSubsystem.h"

// Sets default values
ATartarusHazard::ATartarusHazard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATartarusHazard::BeginPlay()
{
	Super::BeginPlay();

	UTartarusHazardSubsystem* const HazardSubsystem = GetWorld()->GetSubsystem<UTartarusHazardSubsystem>();
	if (!IsValid(HazardSubsystem))
	{
		return;
	}

	HazardSubsystem->RegisterHazard(this);
}

void ATartarusHazard::Activate()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Hazard activated!"));
#endif

	BP_HandleActivated();
}

void ATartarusHazard::Deactivate()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString("Hazard deactivated!"));
#endif

	BP_HandleDeactivated();
}