// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Characters/Player/DOPlayerController.h"

#include "Components/InputComponent.h"
#include "DeckedOut/Components/Interaction/InteractionComponent.h"
#include "DeckedOut/Components/Inventory/InventoryComponent.h"
#include "DeckedOut/UI/Base/PrimaryGameLayout.h"
#include "DeckedOut/World/Items/ItemData.h"
#include "DeckedOut/World/Items/ItemManagerSubsystem.h"

ADOPlayerController::ADOPlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory component", false);
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("Interaction component", false);
}

void ADOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ADOPlayerController::TryInteract);
	InputComponent->BindAction("DEBUG_DropItem", EInputEvent::IE_Pressed, this, &ADOPlayerController::DEBUG_DropItem);
}

void ADOPlayerController::TryInteract()
{
	if (!InteractionComponent->TryInteract(GetPawn()->GetTransform()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Interaction Failed!")));
		
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Interaction Succes!")));
}

void ADOPlayerController::DEBUG_DropItem()
{
	int32 NumItemRetrieved = 0;
	TMap<FString, ItemUniqueDataType> UniqueItemData;

	if (InventoryComponent->RetrieveItems(DEBUG_ItemToDropId, 1, NumItemRetrieved, UniqueItemData))
	{
		UItemManagerSubsystem* const ItemManagerSubsystem = GetWorld()->GetSubsystem<UItemManagerSubsystem>();
		ItemManagerSubsystem->SpawnItem(DEBUG_ItemToDropId, GetPawn()->GetActorTransform());
	}
}

void ADOPlayerController::ShowPrimaryGameLayout()
{
	// [Koen Goossens] TODO: Async loading
	TSubclassOf<UPrimaryGameLayout> PrimaryGameLayoutClassRaw = PrimaryGameLayoutClass.LoadSynchronous();

	if (PrimaryGameLayoutClassRaw)
	{
		PrimaryGameLayoutInstance = CreateWidget<UPrimaryGameLayout>(GetWorld(), PrimaryGameLayoutClassRaw);

		if (PrimaryGameLayoutInstance)
		{
			PrimaryGameLayoutInstance->AddToViewport();
		}
	}
}
