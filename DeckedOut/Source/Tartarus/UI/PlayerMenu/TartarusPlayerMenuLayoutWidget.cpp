// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerMenu/TartarusPlayerMenuLayoutWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "Engine/Engine.h"
#include "Input/CommonUIInputTypes.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"

void UTartarusPlayerMenuLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetupMenuSwitcher();
	RegisterBoundInputActions();
}

void UTartarusPlayerMenuLayoutWidget::SetupMenuSwitcher()
{
	if (MenuSwitcher && VisibilitySwitcher)
	{
		MenuSwitcher->LinkVisibilitySwitcher(VisibilitySwitcher);
	}
}

#pragma region BoundActions
void UTartarusPlayerMenuLayoutWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs ExitBoundActionArguments = FBindUIActionArgs(ExitInputActionData, true, FSimpleDelegate::CreateUObject(this, &UTartarusPlayerMenuLayoutWidget::HandleExitAction));
	ExitBoundActionArguments.bDisplayInActionBar = true;

	ExitActionHandle = RegisterUIActionBinding(ExitBoundActionArguments);
}

void UTartarusPlayerMenuLayoutWidget::HandleExitAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(*FString(__FUNCTION__)));
#endif

	ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (IsValid(PlayerController))
	{
		UTartarusPrimaryGameLayout* const PrimaryGameLayoutInstance = PlayerController->GetPrimaryGameLayout();
		if (PrimaryGameLayoutInstance)
		{
			PrimaryGameLayoutInstance->PopWidgetFromLayer(LayerName, this);
		}
	}
}
#pragma endregion