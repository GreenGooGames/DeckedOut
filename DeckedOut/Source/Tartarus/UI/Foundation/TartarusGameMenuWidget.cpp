// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusGameMenuWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"

void UTartarusGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetupMenuSwitcher();
	RegisterBoundInputActions();
}

void UTartarusGameMenuWidget::SetupMenuSwitcher()
{
	if (MenuSwitcher && VisibilitySwitcher)
	{
		MenuSwitcher->LinkVisibilitySwitcher(VisibilitySwitcher);
	}
}

#pragma region BoundActions
void UTartarusGameMenuWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs ExitBoundActionArguments = FBindUIActionArgs(ExitInputActionData, true, FSimpleDelegate::CreateUObject(this, &UTartarusGameMenuWidget::HandleExitAction));
	ExitBoundActionArguments.bDisplayInActionBar = true;

	ExitActionHandle = RegisterUIActionBinding(ExitBoundActionArguments);
}

void UTartarusGameMenuWidget::HandleExitAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif

	ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (IsValid(PlayerController))
	{
		UTartarusPrimaryGameLayout* const PrimaryGameLayoutInstance = PlayerController->GetPrimaryGameLayout();

		if (PrimaryGameLayoutInstance)
		{
			PrimaryGameLayoutInstance->PopWidgetFromMainStack(*this);
		}
	}
}
#pragma endregion
