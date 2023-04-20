// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidget.h"

#include "CommonLazyImage.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UTartarusInventorySlotWidgetData* const ListItem = Cast<UTartarusInventorySlotWidgetData>(ListItemObject);
	if (IsValid(ListItem) && ListItem->GetTexture().ToSoftObjectPath().IsValid())
	{
		DisplayImage->SetBrushFromLazyTexture(ListItem->GetTexture());
	}
	else
	{
		DisplayImage->SetBrush(EmptySlotBrush);
	}

	// TODO: Should this subscribe to an OnUpdate event of ListItem so that when the data gets updated, this Entry reflects those changes.
}
