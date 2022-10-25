// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/Inventory/InventorySlotWidget.h"

#include "CommonLazyImage.h"
#include "DeckedOut/World/Items/ItemData.h"

void UInventorySlotWidget::SetupDisplayData(const FItemData_Inventory& ItemData)
{
	if (DisplayImage)
	{
		UTexture2D* const DisplayTexture = ItemData.DisplayTexture.Get();

		if (DisplayTexture)
		{
			DisplayImage->SetBrushFromTexture(DisplayTexture);
		}
	}
}

void UInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UInventorySlotWidget* const ListItem = Cast<UInventorySlotWidget>(ListItemObject);

	if (ListItem)
	{
		DisplayImage->SetBrush(ListItem->DisplayImage->Brush);
	}
}