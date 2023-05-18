// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TartarusItemInstance.h"

// Sets default values
ATartarusItemInstance::ATartarusItemInstance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATartarusItemInstance::Initialize(const FPrimaryAssetId ItemReferenceId)
{
	ItemId = ItemReferenceId;
}
