// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusSwitcherWidget.h"

#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"
#include "UI/Foundation/TartarusActivatableWidget.h"
#include "UI/Foundation/TartarusTextButton.h"
#include "UI/Foundation/TartarusSwitcherWidget.h"

void UTartarusSwitcherWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PreviousButton)
	{
		PreviousButton->OnClicked().AddUObject(this, &UTartarusSwitcherWidget::OnPreviousButtonPressed);
	}

	if (NextButton)
	{
		NextButton->OnClicked().AddUObject(this, &UTartarusSwitcherWidget::OnNextButtonPressed);
	}
}

void UTartarusSwitcherWidget::LinkVisibilitySwitcher(TWeakObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher)
{
	if (VisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher = VisibilitySwitcher;
		MenuVisibilitySwitcher->OnActiveWidgetIndexChanged().AddUObject(this, &UTartarusSwitcherWidget::OnMenuVisibilityChanged);
		OnMenuVisibilityChanged(MenuVisibilitySwitcher->GetActiveWidgetIndex());
	}
}

void UTartarusSwitcherWidget::OnPreviousButtonPressed()
{
	if (MenuVisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher->DecrementActiveWidgetIndex();
	}
}

void UTartarusSwitcherWidget::OnNextButtonPressed()
{
	if (MenuVisibilitySwitcher.Get())
	{
		MenuVisibilitySwitcher->IncrementActiveWidgetIndex();
	}
}

void UTartarusSwitcherWidget::OnMenuVisibilityChanged(int32 VisibleWidgetIndex)
{
	if (MenuVisibilitySwitcher.Get() && MenuNameText.Get())
	{
		const UTartarusActivatableWidget* const ActiveWidget = Cast<UTartarusActivatableWidget>(MenuVisibilitySwitcher->GetWidgetAtIndex(VisibleWidgetIndex));

		if (ActiveWidget)
		{
			const int32 NumChildren = MenuVisibilitySwitcher->GetChildrenCount();
			const int32 PreviousIndexRaw = VisibleWidgetIndex - 1;
			const int32 NextIndexRaw = VisibleWidgetIndex + 1;
			const int32 MaxViableIndex = NumChildren - 1;

			const int32 PreviousIndex = PreviousIndexRaw < 0 ? MaxViableIndex : PreviousIndexRaw;
			const int32 NextIndex = NextIndexRaw > MaxViableIndex ? 0 : NextIndexRaw;

			const UTartarusActivatableWidget* const PreviousWidget = Cast<UTartarusActivatableWidget>(MenuVisibilitySwitcher->GetWidgetAtIndex(PreviousIndex));
			const UTartarusActivatableWidget* const NextWidget = Cast<UTartarusActivatableWidget>(MenuVisibilitySwitcher->GetWidgetAtIndex(NextIndex));

			const FText CurrentText = ActiveWidget->GetWidgetName();
			const FText PreviousText = PreviousWidget->GetWidgetName();
			const FText NextText = NextWidget->GetWidgetName();

			MenuNameText->SetText(CurrentText);
			PreviousButton->SetText(PreviousText);
			NextButton->SetText(NextText);
		}
	}
}
