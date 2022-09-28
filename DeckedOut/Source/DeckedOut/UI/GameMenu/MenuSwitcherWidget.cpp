// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/MenuSwitcherWidget.h"

#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"
#include "DeckedOut/UI/Base/CommonTextButton.h"
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

#pragma optimize("", off)
void UMenuSwitcherWidget::OnMenuVisibilityChanged(int32 VisibleWidgetIndex)
{
	if (MenuVisibilitySwitcher.Get() && MenuNameText.Get())
	{
		UWidget* const ActiveWidget = MenuVisibilitySwitcher->GetWidgetAtIndex(VisibleWidgetIndex);

		if (ActiveWidget)
		{
			// [Koen Goossens] TODO: Localize Menu names.
			MenuNameText->SetText(FText::FromName(*ActiveWidget->GetName()));

			const int32 NumChildren = MenuVisibilitySwitcher->GetChildrenCount();
			const int32 PreviousIndexRaw = VisibleWidgetIndex - 1;
			const int32 NextIndexRaw = VisibleWidgetIndex + 1;
			const int32 MaxViableIndex = NumChildren - 1;

			const int32 PreviousIndex = PreviousIndexRaw < 0 ? MaxViableIndex : PreviousIndexRaw;
			const int32 NextIndex = NextIndexRaw > MaxViableIndex ? 0 : NextIndexRaw;

			const UWidget* const PreviousWidget = MenuVisibilitySwitcher->GetWidgetAtIndex(PreviousIndex);
			const UWidget* const NextWidget = MenuVisibilitySwitcher->GetWidgetAtIndex(NextIndex);

			PreviousButton->SetText(FText::FromName(*PreviousWidget->GetName()));
			NextButton->SetText(FText::FromName(*NextWidget->GetName()));
		}
	}
}
#pragma optimize("", on)
