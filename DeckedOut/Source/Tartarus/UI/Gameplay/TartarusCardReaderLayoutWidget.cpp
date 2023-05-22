// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusCardReaderLayoutWidget.h"

#include "UI/Foundation/TartarusActionBarWidget.h"
#include "UI/PlayerMenu/Inventory/TartarusSubInventoryViewWidget.h"
#include "UI/Foundation/TartarusItemSelectionWidget.h"
#include "UI/Foundation/TartarusItemDetailsWidget.h"
#include "UI/Foundation/TartarusGameModifiersWidget.h"

UTartarusCardReaderLayoutWidget::UTartarusCardReaderLayoutWidget()
{
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = true;
	bAutoActivate = true;
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusCardReaderLayoutWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(ActiveCardsWidget))
	{
		return nullptr;
	}
	
	return ActiveCardsWidget;
}

void UTartarusCardReaderLayoutWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// Populate the current Active Cards Widgets.

	// Populate the inventory Widget with the player inventory.

	// Populate the GameModifier Widget with the current Active Cards.
}
#pragma endregion
