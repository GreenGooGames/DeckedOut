// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"

#include "Item.h"

bool FItemData::IsDataValid() const
{
	const bool bIsValidId = Id != INVALID_ItemId;
	const bool bIsValidBlueprint = IsValid(Blueprint.GetDefaultObject());

	return bIsValidId && bIsValidBlueprint;
}
