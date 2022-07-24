// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Gameplay/TreasureManagerSubsystem.h"

#include "DeckedOut/World/Items/Compass.h"
#include "DeckedOut/World/Items/CompassTargetInterface.h"
#include "DeckedOut/World/Treasure/TreasureChest.h"

bool UTreasureManagerSubsystem::SpawnLinkedTreasureChest(TSubclassOf<ATreasureChest> TreasureChestClass, TSoftObjectPtr<ACompass> Compass)
{
	if (SpawnTreasureChest(TreasureChestClass))
	{
		if (LinkTreasureToCompass(SpawnedTreasureChest, Compass))
		{
			return true;
		}
	}

	return false;
}

bool UTreasureManagerSubsystem::SpawnTreasureChest(TSubclassOf<ATreasureChest> TreasureChestClass)
{
	FActorSpawnParameters SpawnParameters;

	SpawnedTreasureChest = GetWorld()->SpawnActor<ATreasureChest>(TreasureChestClass, SpawnParameters);
	
	if (IsValid(SpawnedTreasureChest))
	{
		return true;
	}

	return false;
}

bool UTreasureManagerSubsystem::LinkTreasureToCompass(const TObjectPtr<ATreasureChest> TreasureChest, const TSoftObjectPtr<ACompass> Compass)
{
	ICompassTargetInterface* const CompassTarget = Cast<ICompassTargetInterface>(TreasureChest);

	if (CompassTarget)
	{
		Compass->SetTargetLocation(TreasureChest->GetActorLocation());

		// [Koen Goossens] TODO: Link the compass to the treasure chest and set this compass as a unique key to open the chest.

		return true;
	}

	return false;
}
