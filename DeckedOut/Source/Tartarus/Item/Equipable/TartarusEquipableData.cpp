// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableData.h"

#include "Logging/TartarusLogChannels.h"

#pragma region FEquipmentInfo
void FEquipmentInfo::SetInventoryItemStackId(const FInventoryStackId& InventoryStackIdReference)
{
	if (InventoryStackId.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Overwritting a valid InventoryStackId!"), *FString(__FUNCTION__));
	}

	InventoryStackId = InventoryStackIdReference;
}

void FEquipmentInfo::Reset()
{
	Item = nullptr;
	InventoryStackId = FInventoryStackId();
}
#pragma endregion
