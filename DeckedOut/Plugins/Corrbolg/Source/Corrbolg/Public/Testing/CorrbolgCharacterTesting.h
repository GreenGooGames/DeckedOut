// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "CorrbolgCharacterTesting.generated.h"

class UCorrbolgInventoryManagerComponent;
class UCorrbolgInventoryWidget;
class UInputAction;
class UInputMappingContext;

UCLASS()
class CORRBOLG_API ACorrbolgCharacterTesting : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACorrbolgCharacterTesting();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void BeginPlay() override;

protected:
	/** The manager for the inventories on this character. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TObjectPtr<UCorrbolgInventoryManagerComponent> InventoryManager = nullptr;

	/** Inventory UI Widget class. */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSoftClassPtr<UCorrbolgInventoryWidget> InventoryWidgetClass = nullptr;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	/** Toggle inventory UI Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ToggleInventoryUIAction = nullptr;

private:
	TWeakObjectPtr<UCorrbolgInventoryWidget> InventoryWidgetInstance = nullptr;

	void ToggleInventoryUI();
};
