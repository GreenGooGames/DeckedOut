// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidget.h"

#include "CommonLazyImage.h"
#include "Engine/Texture2D.h"

void UTartarusInventorySlotWidget::SetDisplayTexture(UTexture2D* Texture)
{
	if (IsValid(DisplayImage))
	{
		if (IsValid(Texture))
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

void UTartarusInventorySlotWidget::SetSoftDisplayTexture(TSoftObjectPtr<UTexture2D> Texture)
{
	if (IsValid(DisplayImage))
	{
		// Reset Tint Aplha value to 1.0f
		DisplayImage->Brush.TintColor = FSlateColor(FColor::White);

		DisplayImage->SetBrushFromLazyTexture(Texture);
	}
}

void UTartarusInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UTartarusInventorySlotWidget* const ListItem = Cast<UTartarusInventorySlotWidget>(ListItemObject);

	if (ListItem)
	{
		DisplayImage->SetBrush(ListItem->DisplayImage->Brush);
	}
}
