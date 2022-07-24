// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"

#include "DeckedOut/World/Items/Item.h"

bool FItemData::IsDataValid() const
{
	const bool bIsValidId = Id != INVALID_ItemId;
	const bool bIsValidBlueprint = IsValid(Blueprint.GetDefaultObject());

	return bIsValidId;
}

FItemData_Inventory::FItemData_Inventory(const FItemData& ItemData, const int32 Amount)
{
	Id = ItemData.Id;
	Name = ItemData.Name;
	Description = ItemData.Description;
	DisplayTexture = ItemData.DisplayTexture;
	StackSize = Amount;
}
