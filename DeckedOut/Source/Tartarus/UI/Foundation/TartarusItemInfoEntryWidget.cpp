// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusItemInfoEntryWidget.h"

#include "CommonTextBlock.h"
#include "UI/Foundation/TartarusItemInfoEntryWidgetData.h"

void UTartarusItemInfoEntryWidget::SetText(const FText& InMainText, const FText& InSubText)
{
	MainText->SetText(InMainText);
	SubText->SetText(InSubText);
}

#pragma region IUserObjectListEntry
void UTartarusItemInfoEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UTartarusItemInfoEntryWidgetData* const EntryData = Cast<UTartarusItemInfoEntryWidgetData>(ListItemObject);
	if (!IsValid(EntryData))
	{
		return;
	}

	SetText(EntryData->MainText, EntryData->SubText);
}
#pragma endregion
