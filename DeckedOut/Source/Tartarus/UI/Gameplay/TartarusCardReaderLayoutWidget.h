// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/TartarusStorageLayoutWidget.h"

#include "TartarusCardReaderLayoutWidget.generated.h"

class UTartarusGameModifiersWidget;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCardReaderLayoutWidget : public UTartarusStorageLayoutWidget
{
	GENERATED_BODY()

protected:		
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusGameModifiersWidget> GameModifiersWidget = nullptr;
};
