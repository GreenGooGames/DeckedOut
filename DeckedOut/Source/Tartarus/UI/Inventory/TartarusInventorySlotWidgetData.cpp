// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

#include "Engine/Texture2D.h"

void UTartarusInventorySlotWidgetData::SetTexture(TSoftObjectPtr<UTexture2D> DisplayTexture)
{
	Texture = DisplayTexture;
}

void UTartarusInventorySlotWidgetData::UpdateData(const FInventoryStack* const InventoryStack)
{
	ItemId = InventoryStack ? InventoryStack->GetEntryId() : FTartarusHelpers::InvalidItemId;
	InventoryStackId = InventoryStack ? InventoryStack->GetStackId() : FInventoryStackId();
	StackSize = InventoryStack ? InventoryStack->StackSize : 0;
	Texture = InventoryStack ? Texture : nullptr;

	OnListItemDataChangedEvent.Broadcast(this);
}
