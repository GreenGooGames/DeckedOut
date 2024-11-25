// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/GleipnirItemSubsystem.h"

#include "Engine/World.h"

#include "Data/GleipnirItemAsset.h"
#include "Logging/GleipnirLogChannels.h"

TArray<AActor*> UGleipnirItemSubsystem::SpawnItems(const TArray<TSoftObjectPtr<UGleipnirItemAsset>>& Items, const FTransform& SpawnTransform) const
{
	TArray<AActor*> SpawnedItems;

	if (Items.IsEmpty())
	{
		return SpawnedItems;
	}

	const float Interval = (2.0f * PI) / Items.Num();

	for (const TSoftObjectPtr<UGleipnirItemAsset>& Item : Items)
	{
		const float OrbitAngle = Interval * SpawnedItems.Num();
		const FVector2D UnitOffset = FVector2D(cos(OrbitAngle), sin(OrbitAngle));
		const FVector2D OrbitOffset = UnitOffset * 200.0f; // TODO: Magic Number

		FTransform OrbitTransform = SpawnTransform;
		OrbitTransform.SetLocation(OrbitTransform.GetLocation() + FVector(OrbitOffset, 0));

		const TSubclassOf<AActor> ItemClass = Item.LoadSynchronous()->ItemInstanceClass.LoadSynchronous();
		AActor* const SpawnedItem = SpawnItem(ItemClass, OrbitTransform);

		SpawnedItems.Add(SpawnedItem);
	}

    return SpawnedItems;
}

AActor* UGleipnirItemSubsystem::SpawnItem(const TSubclassOf<AActor>& ItemClass, const FTransform& SpawnTransform) const
{
	const FActorSpawnParameters SpawnParams = FActorSpawnParameters();

	AActor* const ItemInstance = GetWorld()->SpawnActor<AActor>(ItemClass, SpawnTransform, SpawnParams);
	if (!ItemInstance)
	{
		UE_LOG(LogGleipnir, Warning, TEXT("%s: Failed to spawn an item!"), ANSI_TO_TCHAR(__FUNCTION__));
		return nullptr;
	}

	return ItemInstance;
}
