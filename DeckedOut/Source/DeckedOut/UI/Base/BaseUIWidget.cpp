// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/Base/BaseUIWidget.h"

#include "Widgets/CommonActivatableWidgetContainer.h"

void UBaseUIWidget::PushWidgetToMainStack(const TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (MainStack)
	{
		// [Koen Goossens] TODO: ASync loading of class.
		TSubclassOf<UCommonActivatableWidget> WidgetClassRaw = WidgetClass.LoadSynchronous();

		if (IsValid(WidgetClassRaw))
		{
			UCommonActivatableWidget* const NewWidget = MainStack->AddWidget(WidgetClassRaw);
			NewWidget->ActivateWidget();

			OnWidgetPushedToMainStack(NewWidget);
		}
	}
}

void UBaseUIWidget::PopWidgetFromMainStack(UCommonActivatableWidget& WidgetInstance)
{
	if (MainStack)
	{
		MainStack->RemoveWidget(WidgetInstance);

		OnWidgetPoppedFromMainStack();
	}
}

void UBaseUIWidget::OnWidgetPushedToMainStack(const UCommonActivatableWidget* const Widget)
{
	APlayerController* const PlayerController = GetOwningPlayer();

	if (Widget && PlayerController)
	{
		FInputModeUIOnly InputModeUIOnlyParameters = FInputModeUIOnly();
		UWidget* const DesiredFocusTarget = Widget->GetDesiredFocusTarget();

		if (DesiredFocusTarget)
		{
			InputModeUIOnlyParameters.SetWidgetToFocus(DesiredFocusTarget->TakeWidget());
		}

		PlayerController->SetInputMode(InputModeUIOnlyParameters);
		PlayerController->SetShowMouseCursor(true);
	}
}

void UBaseUIWidget::OnWidgetPoppedFromMainStack()
{
	// If the Mainstack doesn't contain any other widgets (other than the gameplayHUD, reveal the Base UI again.
	if (MainStack->GetNumWidgets() <= 1)
	{
		APlayerController* const PlayerController = GetOwningPlayer();

		if (PlayerController)
		{		
			FInputModeGameOnly InputModeGameOnlyParameters = FInputModeGameOnly();
			PlayerController->SetInputMode(InputModeGameOnlyParameters);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

