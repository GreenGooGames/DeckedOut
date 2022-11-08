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

			// Reset Tint Aplha value to 1.0f
			DisplayImage->Brush.TintColor = FSlateColor(FColor::White);
		}
		else
		{
			DisplayImage->SetBrush(EmptySlotBrush);
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
