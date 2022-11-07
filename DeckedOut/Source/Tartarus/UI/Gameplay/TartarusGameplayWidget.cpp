// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusGameplayWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"

void UTartarusGameplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
}

#pragma region BoundActions
void UTartarusGameplayWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs OpenGameMenuBoundActionArguments = FBindUIActionArgs(OpenGameMenuInputActionData, true, FSimpleDelegate::CreateUObject(this, &UTartarusGameplayWidget::HandleOpenGameMenuAction));
	OpenGameMenuBoundActionArguments.bDisplayInActionBar = true;

	OpenGameMenuActionHandle = RegisterUIActionBinding(OpenGameMenuBoundActionArguments);
}

void UTartarusGameplayWidget::HandleOpenGameMenuAction() const
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif

	ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Open Game Menu failed: No player controller!"), __FUNCTION__);
		return;
	}

	UTartarusPrimaryGameLayout* const PrimaryGameLayoutInstance = PlayerController->GetPrimaryGameLayout();

	if (!IsValid(PrimaryGameLayoutInstance))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Open Game Menu failed: No Primary gamelayout found!"), __FUNCTION__);
		return;
	}

	FPushedToStackEvent OnRequestCompleted = FPushedToStackEvent();

	PrimaryGameLayoutInstance->AsyncRequestPushToStack(GameMenuClass, OnRequestCompleted);
}
#pragma endregion
