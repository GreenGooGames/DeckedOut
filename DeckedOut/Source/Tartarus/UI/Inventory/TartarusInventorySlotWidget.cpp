// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidget.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UTartarusInventorySlotWidgetData* const ListItem = Cast<UTartarusInventorySlotWidgetData>(ListItemObject);
	if (!IsValid(ListItem))
	{
		return;
	}

	// Set the texture to display that represents the ListItem.
	if (ListItem->GetTexture().ToSoftObjectPath().IsValid())
	{
		DisplayImage->SetBrushFromLazyTexture(ListItem->GetTexture()); // CommonLazy will ASync load the iamge on its own and display a loading image.
	}
	else
	{
		DisplayImage->SetBrush(EmptySlotBrush);
	}

	// Display how many of the ListItem are in the inventory.
	if (ListItem->GetStackSize() > 0)
	{
		const FText TextToDisplay = FText::FromString(FString("x").Append(FString::FromInt(ListItem->GetStackSize())));
		StackSizeText->SetText(TextToDisplay);
	}
	else
	{
		const FText TextToDisplay = FText();
		StackSizeText->SetText(TextToDisplay);
	}

	// TODO: Should this subscribe to an OnUpdate event of ListItem so that when the data gets updated, this Entry reflects those changes.
}
