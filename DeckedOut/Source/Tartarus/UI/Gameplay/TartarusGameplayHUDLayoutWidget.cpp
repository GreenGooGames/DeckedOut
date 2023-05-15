// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusGameplayHUDLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"

void UTartarusGameplayHUDLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
}

#pragma region UCommonActivatableWidget
TOptional<FUIInputConfig> UTartarusGameplayHUDLayoutWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CapturePermanently);
}
#pragma endregion

#pragma region BoundActions
void UTartarusGameplayHUDLayoutWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs OpenPlayerMenuBoundActionArguments = FBindUIActionArgs(OpenPlayerMenuEnhancedInputAction, true, FSimpleDelegate::CreateUObject(this, &UTartarusGameplayHUDLayoutWidget::HandleOpenPlayerMenuAction));
	OpenPlayerMenuBoundActionArguments.bDisplayInActionBar = true;

	OpenPlayerMenuActionHandle = RegisterUIActionBinding(OpenPlayerMenuBoundActionArguments);
}

void UTartarusGameplayHUDLayoutWidget::HandleOpenPlayerMenuAction() const
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Open Game Menu failed: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusPrimaryGameLayout* const PrimaryGameLayoutInstance = PlayerController->GetPrimaryGameLayout();
	if (!IsValid(PrimaryGameLayoutInstance))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Open Game Menu failed: No Primary gamelayout found!"), *FString(__FUNCTION__));
		return;
	}

	PrimaryGameLayoutInstance->PushWidgetToLayerAsync(PlayerMenuLayer, PlayerMenuClass);
}
#pragma endregion