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

bool ATartarusTreasureHuntEntrance::CanChangeState(const EDoorState NewState) const
{
	const bool bIsDifferentState = Super::CanChangeState(NewState);

	if (!bIsDifferentState)
	{
		return false;
	}

	// To change the state from Closed --> Open, a treasure hunt has to be activated.
	if (NewState == EDoorState::Open)
	{
		// Is the game mode correct?
		ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

		// Start the treasure hunt.
		GameMode->StartTreasureHunt();

		ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();

		if (!GameState->IsTreasureHuntActive())
		{
			return false;
		}
	}
	// To change the state fom Open --> Closed, the active treasure hunt has to be deactivated.
	else if (NewState == EDoorState::Closed)
	{
		// Is the game mode correct?
		ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

		// Stop the treasure hunt.
		GameMode->StopTreasureHunt();

		ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();

		if (GameState->IsTreasureHuntActive())
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
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
