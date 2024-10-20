// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prefabs/CorrbolgNavigationWidget.h"

#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"

#include "UI/Prefabs/CorrbolgTextButton.h"
#include "Utilities/CorrbolgMathUtilities.h"

void UCorrbolgNavigationWidget::SetupSwitcher(UCommonVisibilitySwitcher* const VisibilitySwitcher)
{
	if (IsValid(PreviousEntryButton) && !PreviousEntryButton->OnClicked().IsBound())
	{
		PreviousEntryButton->OnClicked().AddUObject(this, &UCorrbolgNavigationWidget::OnPreviousButtonPressed);
	}
	
	if (IsValid(NextEntryButton) && !NextEntryButton->OnClicked().IsBound())
	{
		NextEntryButton->OnClicked().AddUObject(this, &UCorrbolgNavigationWidget::OnNextButtonPressed);
	}

	if (IsValid(VisibilitySwitcher))
	{
		EntryVisibilitySwitcher = VisibilitySwitcher;

		EntryVisibilitySwitcher->OnActiveWidgetIndexChanged().AddUObject(this, &UCorrbolgNavigationWidget::OnEntryVisibilityChanged);
		OnEntryVisibilityChanged(EntryVisibilitySwitcher->GetActiveWidgetIndex());
	}
}

void UCorrbolgNavigationWidget::OnPreviousButtonPressed()
{
	if (EntryVisibilitySwitcher.IsValid())
	{
		EntryVisibilitySwitcher->DecrementActiveWidgetIndex();
	}
}

void UCorrbolgNavigationWidget::OnNextButtonPressed()
{
	if (EntryVisibilitySwitcher.IsValid())
	{
		EntryVisibilitySwitcher->IncrementActiveWidgetIndex();
	}
}

void UCorrbolgNavigationWidget::OnEntryVisibilityChanged(int32 VisibleWidgetIndex)
{
	if (!EntryVisibilitySwitcher.IsValid() || !IsValid(PreviousEntryButton) || !IsValid(NextEntryButton))
	{
		return;
	}

	// Get the relevant indices and their associated widgets.
	const int32 PreviousIndex = FCorrbolgMathUtilities::Wrap(VisibleWidgetIndex - 1, 0, EntryVisibilitySwitcher->GetChildrenCount() - 1);
	const int32 NextIndex = FCorrbolgMathUtilities::Wrap(VisibleWidgetIndex + 1, 0, EntryVisibilitySwitcher->GetChildrenCount() - 1);

	UWidget* const ActiveWidget = EntryVisibilitySwitcher->GetActiveWidget();
	UWidget* const PreviousWidget = EntryVisibilitySwitcher->GetWidgetAtIndex(PreviousIndex);
	UWidget* const NextWidget = EntryVisibilitySwitcher->GetWidgetAtIndex(NextIndex);

	if (!IsValid(ActiveWidget) || !IsValid(PreviousWidget) || !IsValid(NextWidget))
	{
		return;
	}

	// TODO: Get a localized text from the widgets to disaply on the button and title.
	// Set the text to display as title and on the buttons.
	EntryTitleText->SetText(FText::FromString(ActiveWidget->GetName()));
	PreviousEntryButton->SetText(FText::FromString(PreviousWidget->GetName()));
	NextEntryButton->SetText(FText::FromString(NextWidget->GetName()));
}
