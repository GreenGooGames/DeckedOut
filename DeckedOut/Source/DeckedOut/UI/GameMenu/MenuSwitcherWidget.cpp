// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/MenuSwitcherWidget.h"

#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"
#include "MenuSwitcherWidget.h"

void UMenuSwitcherWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PreviousButton)
	{
		PreviousButton->OnClicked().AddUObject(this, &UMenuSwitcherWidget::OnPreviousButtonPressed);
	}
	
	if (NextButton)
	{
		NextButton->OnClicked().AddUObject(this, &UMenuSwitcherWidget::OnNextButtonPressed);
	}
}

void UMenuSwitcherWidget::LinkVisibilitySwitcher(TWeakObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher)
{
	if (VisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher = VisibilitySwitcher;
		MenuVisibilitySwitcher->OnActiveWidgetIndexChanged().AddUObject(this, &UMenuSwitcherWidget::OnMenuVisibilityChanged);
		OnMenuVisibilityChanged(MenuVisibilitySwitcher->GetActiveWidgetIndex());
	}
}

void UMenuSwitcherWidget::OnPreviousButtonPressed()
{
	if (MenuVisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher->DecrementActiveWidgetIndex();
	}
}

void UMenuSwitcherWidget::OnNextButtonPressed()
{
	if (MenuVisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher->IncrementActiveWidgetIndex();
	}
}

void UMenuSwitcherWidget::OnMenuVisibilityChanged(int32 VisibleWidgetIndex)
{
	if (MenuVisibilitySwitcher.Get() && MenuNameText.Get())
	{
		UWidget* const ActiveWidget = MenuVisibilitySwitcher->GetWidgetAtIndex(VisibleWidgetIndex);

		if (ActiveWidget)
		{
			// [Koen Goossens] TODO: Localize Menu names.
			MenuNameText->SetText(FText::FromName(*ActiveWidget->GetName()));
		}
	}
}
