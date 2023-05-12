// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerMenu/TartarusPlayerMenuLayoutWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"

void UTartarusPlayerMenuLayoutWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetupMenuSwitcher();
}

void UTartarusPlayerMenuLayoutWidget::SetupMenuSwitcher()
{
	if (MenuSwitcher && VisibilitySwitcher)
	{
		MenuSwitcher->LinkVisibilitySwitcher(VisibilitySwitcher);
	}
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusPlayerMenuLayoutWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(VisibilitySwitcher))
	{
		return nullptr;
	}

	UWidget* const WidgetToFocus = VisibilitySwitcher->GetActiveWidget();
	return WidgetToFocus;
}
#pragma endregion
