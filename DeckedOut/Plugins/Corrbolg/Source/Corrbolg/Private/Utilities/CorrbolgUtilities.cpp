// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/CorrbolgUtilities.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#include "Inventory/CorrbolgInventoryManagerComponent.h"

UCorrbolgInventoryManagerComponent* FCorrbolgUtilities::FindInventoryComponent(const APlayerController* const PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	UCorrbolgInventoryManagerComponent* InventoryComponent = PlayerController->FindComponentByClass<UCorrbolgInventoryManagerComponent>();
	if (!IsValid(InventoryComponent))
	{
		InventoryComponent = PlayerController->GetPawn()->FindComponentByClass<UCorrbolgInventoryManagerComponent>();
		if (!IsValid(InventoryComponent))
		{
			return nullptr;
		}
	}

	return InventoryComponent;
}
