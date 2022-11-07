// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidget.h"

#include "CommonLazyImage.h"

void UTartarusInventorySlotWidget::SetDisplayTexture(UTexture2D* Texture)
{
	if (DisplayImage)
	{
		if (Texture)
		{
			DisplayImage->SetBrushFromTexture(Texture);
		}
	}
}

void UTartarusInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UTartarusInventorySlotWidget* const ListItem = Cast<UTartarusInventorySlotWidget>(ListItemObject);

	if (ListItem)
	{
		DisplayImage->SetBrush(ListItem->DisplayImage->Brush);
	}
}
