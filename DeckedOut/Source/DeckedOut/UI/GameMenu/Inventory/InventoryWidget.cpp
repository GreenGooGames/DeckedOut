// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/Inventory/InventoryWidget.h"
#include "Input/CommonUIInputTypes.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
}

#pragma region BoundActions
void UInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SelectBoundActionArguments = FBindUIActionArgs(SelectInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectAction));
	SelectBoundActionArguments.bDisplayInActionBar = true;

	SelectActionHandle = RegisterUIActionBinding(SelectBoundActionArguments);

	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UInventoryWidget::HandleSelectAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}

void UInventoryWidget::HandleSortAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}
#pragma endregion