// Fill out your copyright notice in the Description page of Project Settings.


#include "Testing/CorrbolgCharacterTesting.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Inventory/CorrbolgInventoryManagerComponent.h"
#include "UI/Inventory/CorrbolgInventoryWidget.h"

ACorrbolgCharacterTesting::ACorrbolgCharacterTesting()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryManager = CreateDefaultSubobject<UCorrbolgInventoryManagerComponent>(FName("Inventory Manager"));
}

void ACorrbolgCharacterTesting::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* const EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Toggle Invnetory UI widget.
		EnhancedInputComponent->BindAction(ToggleInventoryUIAction, ETriggerEvent::Completed, this, &ACorrbolgCharacterTesting::ToggleInventoryUI);
	}
}

void ACorrbolgCharacterTesting::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (const APlayerController* const PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* const Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACorrbolgCharacterTesting::ToggleInventoryUI()
{
	if (InventoryWidgetInstance.IsValid() && InventoryWidgetInstance->IsInViewport())
	{
		// Remove the widget if it exists.
		InventoryWidgetInstance->RemoveFromParent();
		return;
	}
	else if (InventoryWidgetInstance.IsValid())
	{
		// Re-add the widget if it exists.
		InventoryWidgetInstance->AddToViewport();
		return;
	}
	else
	{
		// Create and add the widget to the viewport.
		InventoryWidgetInstance = CreateWidget<UCorrbolgInventoryWidget>(GetWorld(), InventoryWidgetClass.LoadSynchronous());
		if (!IsValid(InventoryWidgetInstance.Get()))
		{
			return;
		}

		InventoryWidgetInstance->AddToViewport();
	}
}

