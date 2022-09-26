// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/Gameplay/GameplayWidget.h"

#include "DeckedOut/Characters/Player/DOPlayerController.h"
#include "DeckedOut/UI/Base/BaseUIWidget.h"
#include "Input/CommonUIInputTypes.h"

void UGameplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
}

#pragma region BoundActions
void UGameplayWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs OpenGameMenuBoundActionArguments = FBindUIActionArgs(OpenGameMenuInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleOpenGameMenuAction));
	OpenGameMenuBoundActionArguments.bDisplayInActionBar = true;

	OpenGameMenuActionHandle = RegisterUIActionBinding(OpenGameMenuBoundActionArguments);
}

void UGameplayWidget::HandleOpenGameMenuAction() const
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif

	ADOPlayerController* const PlayerController = GetOwningPlayer<ADOPlayerController>();

	if (PlayerController)
	{
		UBaseUIWidget* const UIInstance = PlayerController->GetUIInstance();

		if (UIInstance)
		{
			UIInstance->PushWidgetToMainStack(GameMenuClass);
		}
	}
}
#pragma endregion