// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeckedOutGameMode.h"
#include "DeckedOutCharacter.h"
#include "DeckedOut/Characters/Player/DOPlayerController.h"

#include "UObject/ConstructorHelpers.h"

ADeckedOutGameMode::ADeckedOutGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ADeckedOutGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ADOPlayerController* const PlayerController = Cast<ADOPlayerController>(NewPlayer);

	if (IsValid(PlayerController))
	{
		PlayerController->ShowPrimaryGameLayout();
	}
}
