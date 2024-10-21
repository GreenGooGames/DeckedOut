// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prefabs/CorrbolgNavigationWidget.h"

#include "CommonTextBlock.h"
#include "CommonVisibilitySwitcher.h"

#include "UI/Prefabs/CorrbolgTextButton.h"
#include "Utilities/CorrbolgMathUtilities.h"
#include "UI/Inventory/CorrbolgInventoryViewWidget.h"

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

	const UCorrbolgInventoryViewWidget* const ActiveWidget = Cast<UCorrbolgInventoryViewWidget>(EntryVisibilitySwitcher->GetActiveWidget());
	const UCorrbolgInventoryViewWidget* const PreviousWidget = Cast<UCorrbolgInventoryViewWidget>(EntryVisibilitySwitcher->GetWidgetAtIndex(PreviousIndex));
	const UCorrbolgInventoryViewWidget* const NextWidget = Cast<UCorrbolgInventoryViewWidget>(EntryVisibilitySwitcher->GetWidgetAtIndex(NextIndex));

	if (!IsValid(ActiveWidget) || !IsValid(PreviousWidget) || !IsValid(NextWidget))
	{
		return;
	}

	// Set the text to display as title and on the buttons.
	EntryTitleText->SetText(ActiveWidget->GetDisplayText());
	PreviousEntryButton->SetText(PreviousWidget->GetDisplayText());
	NextEntryButton->SetText(NextWidget->GetDisplayText());
}
