// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/System/TartarusAsyncItemData.h"

#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Item/TartarusItemInstance.h"

#pragma region FSpawnItemsRequestInfo
FSpawnItemsRequestInfo::FSpawnItemsRequestInfo(const FTransform& Transform, const FItemSpawnParameters& ItemSpawnParameters, const FItemSpawnRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	SpawnTransform = Transform;
	SpawnParameters = ItemSpawnParameters;

	RequestCompletedEvent = OnCompleted;
}

void FSpawnItemsRequestInfo::AddItemToLoad(const FSoftObjectPath& ItemObjectPath, const FPrimaryAssetId ItemId)
{
	FLoadingItemData* const  ExistingLoad = ItemsToLoad.FindByPredicate([&ItemId](const FLoadingItemData& ItemData)
		{
			return ItemData.ItemId == ItemId;
		});

	if (ExistingLoad) // Increase the count of instances to spawn of this Item.
	{
		ExistingLoad->Count++;
	}
	else // Create a new entry and add it.
	{
		FLoadingItemData LoadData = FLoadingItemData();
		LoadData.ItemId = ItemId;
		LoadData.ObjectPath = ItemObjectPath;
		LoadData.Count = 1;

		ItemsToLoad.Add(LoadData);
	}
}

TArray<FSoftObjectPath> FSpawnItemsRequestInfo::GetAssetsToLoad() const
{
	TArray<FSoftObjectPath> Assets;

	for (const FLoadingItemData& ItemData : ItemsToLoad)
	{
		Assets.Add(ItemData.ObjectPath);
	}

	return Assets;
}

FPrimaryAssetId FSpawnItemsRequestInfo::GetItemId(const UObject* const Asset) const
{
	for (const FLoadingItemData& ItemData : ItemsToLoad)
	{
		if (ItemData.ObjectPath.ResolveObject() == Asset)
		{
			return ItemData.ItemId;
		}
	}

	return FTartarusHelpers::InvalidItemId;
}
#pragma endregion

#pragma region FGetItemDataRequestInfo
FGetItemDataRequestInfo::FGetItemDataRequestInfo(const TArray<FPrimaryAssetId>& ItemIdsToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	ItemIds = ItemIdsToLoad;
	RequestCompletedEvent = OnCompleted;
}

FGetItemDataRequestInfo::FGetItemDataRequestInfo(const TArray<EItemType>& ItemTypesToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted)
{
	RequestId = FGuid::NewGuid();

	ItemTypes = ItemTypesToLoad;
	RequestCompletedEvent = OnCompleted;
}
#pragma endregion