// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CorrbolgInventoryViewEntryWidget.h"
#include "UI/Inventory/CorrbolgInventoryViewListItem.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "Engine/Texture2D.h"
#include "Inventory/CorrbolgInventoryEntry.h"

void UCorrbolgInventoryViewEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UCorrbolgInventoryViewListItem* const ListItem = Cast<UCorrbolgInventoryViewListItem>(ListItemObject);
	if (!IsValid(ListItem))
	{
		return;
	}

	// TODO: Do something if no texture is provided.
	if(ListItem->GetTexture().IsValid())
	{
		DisplayImage->SetBrushFromLazyTexture(ListItem->GetTexture());
	}

	// TODO: Hide the text if the entry is an empty inventory entry.
	const FText StackSizeToDisplay = FText::FromString(FString("x").Append(FString::FromInt(ListItem->GetInventoryEntry()->StackSize)));
	StackSizeText->SetText(StackSizeToDisplay);
}
