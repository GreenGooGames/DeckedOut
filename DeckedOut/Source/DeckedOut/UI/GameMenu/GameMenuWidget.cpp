// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/GameMenuWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "DeckedOut/Characters/Player/DOPlayerController.h"
#include "DeckedOut/UI/Base/BaseUIWidget.h"
#include "DeckedOut/UI/GameMenu/MenuSwitcherWidget.h"
#include "Input/CommonUIInputTypes.h"


void UGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetupMenuSwitcher();
	RegisterBoundInputActions();
}

void UGameMenuWidget::SetupMenuSwitcher()
{
	if (MenuSwitcher && VisibilitySwitcher)
	{
		MenuSwitcher->LinkVisibilitySwitcher(VisibilitySwitcher);
	}
}

#pragma region BoundActions
void UGameMenuWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs ExitBoundActionArguments = FBindUIActionArgs(ExitInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleExitAction));
	ExitBoundActionArguments.bDisplayInActionBar = true;

	ExitActionHandle = RegisterUIActionBinding(ExitBoundActionArguments);
}

void UGameMenuWidget::HandleExitAction()
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
			UIInstance->PopWidgetFromMainStack(*this);
		}
	}
}
#pragma endregion