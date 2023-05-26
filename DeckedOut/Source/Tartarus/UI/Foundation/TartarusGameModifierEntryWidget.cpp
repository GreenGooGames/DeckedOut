// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusGameModifierEntryWidget.h"
#include "CommonTextBlock.h"
#include "CommonLazyImage.h"
#include "UI/Foundation/TartarusGameModifierEntryData.h"

void UTartarusGameModifierEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UTartarusGameModifierEntryData* const EntryData = Cast<UTartarusGameModifierEntryData>(ListItemObject);
	if (!IsValid(EntryData))
	{
		return;
	}

	ModifierName->SetText(EntryData->ModifierName);
	ModifierValue->SetText(FText::FromString(FString::SanitizeFloat(EntryData->ModifierWeight)));

	ToolTipTrigger->SetToolTipText(EntryData->ModifierTooltip);
}
