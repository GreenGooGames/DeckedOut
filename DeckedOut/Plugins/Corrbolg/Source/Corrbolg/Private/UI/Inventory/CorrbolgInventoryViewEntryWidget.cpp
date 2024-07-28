// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryViewEntryWidget.h"
#include "UI/Inventory/CorrbolgInventoryViewListItem.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"

void UCorrbolgInventoryViewEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UCorrbolgInventoryViewListItem* const ListItem = Cast<UCorrbolgInventoryViewListItem>(ListItemObject);
	if (!IsValid(ListItem))
	{
		return;
	}

	DisplayImage->SetBrushFromLazyTexture(ListItem->GetTexture());

	const FText StackSizeToDisplay = FText::FromString(FString("x").Append(FString::FromInt(ListItem->GetStackSize())));
	StackSizeText->SetText(StackSizeToDisplay);
}
