// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

#include "Engine/Texture2D.h"

void UTartarusInventorySlotWidgetData::SetTexture(TSoftObjectPtr<UTexture2D> DisplayTexture)
{
	Texture = DisplayTexture;

	OnListItemDataChangedEvent.Broadcast(this);
}

void UTartarusInventorySlotWidgetData::UpdateData(const FInventoryStack* const InventoryStack)
{
	Texture = InventoryStack ? (InventoryStack->GetEntryId() == ItemId ? Texture : nullptr) : nullptr;

	ItemId = InventoryStack ? InventoryStack->GetEntryId() : FTartarusHelpers::InvalidItemId;
	InventoryStackId = InventoryStack ? InventoryStack->GetStackId() : FInventoryStackId();
	StackSize = InventoryStack ? InventoryStack->StackSize : 0;

	OnListItemDataChangedEvent.Broadcast(this);
}
