// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionEntryWidget.h"
#include "CommonTextBlock.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionListItem.h"

void UCorrbolgContextActionEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UCorrbolgContextActionListItem* const ListItem = Cast<UCorrbolgContextActionListItem>(ListItemObject);
	if (!IsValid(ListItem))
	{
		return;
	}

	TextBlock->SetText(ListItem->GetDisplayText());
}
