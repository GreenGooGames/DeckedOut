// Copyright Epic Games, Inc. All Rights Reserved.

#include "TartarusGameMode.h"

#include "Character/TartarusCharacter.h"
#include "Player/TartarusPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATartarusGameMode::ATartarusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Template/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATartarusGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATartarusPlayerController* const PlayerController = Cast<ATartarusPlayerController>(NewPlayer);

	if (IsValid(PlayerController))
	{
		PlayerController->ShowPrimaryGameLayoutASync();
	}
}