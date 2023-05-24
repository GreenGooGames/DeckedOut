// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cards/TartarusCardReader.h"
#include "Item/Inventory/TartarusInventoryComponent.h"

ATartarusCardReader::ATartarusCardReader()
{
	Inventory = CreateDefaultSubobject<UTartarusInventoryComponent>("InventoryComponent");
}
