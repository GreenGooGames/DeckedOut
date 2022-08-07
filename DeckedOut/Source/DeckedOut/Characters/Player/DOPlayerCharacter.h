// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "DeckedOut/DeckedOutCharacter.h"
#include "DOPlayerCharacter.generated.h"

class UEquipmentManager;

UCLASS()
class DECKEDOUT_API ADOPlayerCharacter : public ADeckedOutCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADOPlayerCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEquipmentManager> EquipmentManager = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 DEBUG_CompassID = 0;

	UFUNCTION()
	void DEBUG_EquipCompass();
	UFUNCTION()
	void DEBUG_UnequipCompass();
};
