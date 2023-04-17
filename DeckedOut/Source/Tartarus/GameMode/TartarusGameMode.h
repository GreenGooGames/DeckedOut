// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "TartarusGameMode.generated.h"

UCLASS(minimalapi)
class ATartarusGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATartarusGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



