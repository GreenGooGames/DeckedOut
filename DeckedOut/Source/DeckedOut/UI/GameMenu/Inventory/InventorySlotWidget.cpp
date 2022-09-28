// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/Inventory/InventorySlotWidget.h"

#include "CommonLazyImage.h"
#include "DeckedOut/World/Items/ItemData.h"

void UInventorySlotWidget::SetupDisplayData(const FItemData_Inventory& ItemData)
{
	if (DisplayImage)
	{
		// [Koen Goossens] TODO: This doesn't seem to work, the texture displayed remains unchanged. (The BP call (SetBrushFromTexture) does work investigate the BP method).
		//		Its also possible that the default image finishes laoding after this has executed and then overrides.
		UTexture2D* const DisplayTexture = ItemData.DisplayTexture.Get();

		if (DisplayTexture)
		{
			DisplayImage->SetBrushFromTexture(DisplayTexture);
		}
	}
}