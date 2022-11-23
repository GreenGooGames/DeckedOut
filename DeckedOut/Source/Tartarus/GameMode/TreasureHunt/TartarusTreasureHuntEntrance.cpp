// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntEntrance.h"

#include "Components/BoxComponent.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Logging/TartarusLogChannels.h"

ATartarusTreasureHuntEntrance::ATartarusTreasureHuntEntrance()
{
	// Close Trigger.
	CloseTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Closing Trigger"));
	CloseTrigger->SetSimulatePhysics(false);
	CloseTrigger->SetCollisionProfileName(FName("Trigger"), false);
	CloseTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATartarusTreasureHuntEntrance::OnCloseTriggerOverlap);

	CloseTrigger->SetupAttachment(RootComponent);
}

void ATartarusTreasureHuntEntrance::HandleStateChanged(const EDoorState NewState, AController* const InstigatorController)
{
	Super::HandleStateChanged(NewState, InstigatorController);

	// Skip if a wrong state is received.
	if (NewState == EDoorState::None)
	{
		return;
	}

	// Is the game mode correct?
	ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

	if (!IsValid(GameMode))
	{
		return;
	}

	switch (NewState)
	{
	case EDoorState::Open:
	{
		// Start the treasure hunt.
		GameMode->StartTreasureHunt();

		break;
	}
	case EDoorState::Closed:
	{
		// Stop the treasure hunt.
		GameMode->StopTreasureHunt();
		break;
	}
	// Intentioanl fall-trough.
	case EDoorState::None:
	case EDoorState::Blocked:
	default:
		// Not implemented.
		break;
	}
}

void ATartarusTreasureHuntEntrance::OnCloseTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the OtherActor is the player.
	APawn* const OtherPawn = Cast<APawn>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (!OtherPawn->IsPlayerControlled())
	{
		return;
	}

	// Check if the game is in progress.
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();

	if (!IsValid(GameState) || !GameState->IsTreasureHuntActive())
	{
		return;
	}

	// Close the door.
	EDoorState NewState = EDoorState::Closed;
	bool bHasStateChanged = ChangeState(NewState);

	if (bHasStateChanged)
	{
		HandleStateChanged(NewState, OtherPawn->GetController());
	}
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusTreasureHuntEntrance::IsInteractable() const
{
	bool bIsInteractable = Super::IsInteractable();

	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();

	if (!IsValid(GameState))
	{
		return false;
	}

	bIsInteractable = bIsInteractable && !GameState->IsTreasureHuntActive();

	return bIsInteractable;
}
#pragma endregion
