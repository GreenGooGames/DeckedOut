// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntEntrance.h"

#include "Components/BoxComponent.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Logging/TartarusLogChannels.h"

void ATartarusTreasureHuntEntrance::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Collider->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATartarusTreasureHuntEntrance::OnColliderBeginOverlap);
	Collider->OnComponentEndOverlap.AddUniqueDynamic(this, &ATartarusTreasureHuntEntrance::OnColliderEndOverlap);
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

void ATartarusTreasureHuntEntrance::OnColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

	// Try changing the state of the door.
	const EDoorState NewState = EDoorState::Open;
	
	const bool bHasStateChanged = ChangeState(NewState);
	if (bHasStateChanged)
	{
		HandleStateChanged(NewState, OtherPawn->GetController());
		GenerateNoise();
	}
}

void ATartarusTreasureHuntEntrance::OnColliderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

	// Check if the player is outside the game area.
	const FVector2D CenterToPlayer = FVector2D(OtherPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const FVector2D Forward2D = FVector2D(GetActorForwardVector()).GetSafeNormal();
	const float DotResult = CenterToPlayer.Dot(Forward2D);

	if (DotResult < 0) // Dot smaller than 0 means the Pawn is behind the center and thus still in the dungeon.
	{
		return;
	}

	// Try changing the state of the door.
	const EDoorState NewState = EDoorState::Closed;

	const bool bHasStateChanged = ChangeState(NewState);
	if (bHasStateChanged)
	{
		HandleStateChanged(NewState, OtherPawn->GetController());
		GenerateNoise();
	}
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusTreasureHuntEntrance::IsInteractable() const
{
	return false;
}
#pragma endregion
