// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Objects/TartarusInteractablePopupActor.h"

#include "TartarusCardReader.generated.h"

class UTartarusInventoryComponent;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusCardReader : public ATartarusInteractablePopupActor
{
	GENERATED_BODY()
	
public:
	ATartarusCardReader();

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> Inventory = nullptr;
};
