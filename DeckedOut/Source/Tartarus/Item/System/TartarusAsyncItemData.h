// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/TartarusASyncLoadData.h"
#include "System/TartarusHelpers.h"

#include "TartarusASyncItemData.Generated.h"

enum class EItemType : uint8;

class ATartarusItemInstance;
class UTartarusItem;

#pragma region ASyncSpawn
UENUM(BlueprintType)
enum class EItemSpawnMethod : uint8
{
	Point,	// On a point
	Orbit	// Around a point
};

USTRUCT(BlueprintType)
struct FItemSpawnParameters
{
	GENERATED_BODY()

	EItemSpawnMethod GetSpawnMethod() const { return SpawnMethod; }
	float GetOrbitRadius() const { return OrbitRadius; }

protected:
	UPROPERTY(EditDefaultsOnly)
		EItemSpawnMethod SpawnMethod = EItemSpawnMethod::Point;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnMethod == EItemSpawnMethod::Orbit", EditConditionHides))
		float OrbitRadius = 250.0f;
};

DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FItemSpawnRequestCompletedEvent, FGuid /*RequestId*/, TArray<TWeakObjectPtr<ATartarusItemInstance>> /*SpawnedItems*/)

USTRUCT()
struct FLoadingItemData
{
	GENERATED_BODY()

public:
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;
	int32 Count = 0;
	FSoftObjectPath ObjectPath = FSoftObjectPath();
};

USTRUCT()
struct FSpawnItemsRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FSpawnItemsRequestInfo() {}
	FSpawnItemsRequestInfo(const FTransform& Transform, const FItemSpawnParameters& ItemSpawnParameters, const FItemSpawnRequestCompletedEvent& OnCompleted);

	void AddItemToLoad(const FSoftObjectPath& ItemObjectPath, const FPrimaryAssetId ItemId);
	TArray<FSoftObjectPath> GetAssetsToLoad() const;
	const TArray<FLoadingItemData>& GetItemsToLoad() const { return ItemsToLoad; };

	FPrimaryAssetId GetItemId(const UObject* const Asset) const;

	const FTransform& GetSpawnTransform() const { return SpawnTransform; }
	const FItemSpawnParameters GetSpawnParameters() const { return SpawnParameters; }

	const FItemSpawnRequestCompletedEvent& OnItemSpawnRequestCompleted() const { return RequestCompletedEvent; }

private:
	FTransform SpawnTransform = FTransform();
	FItemSpawnParameters SpawnParameters = FItemSpawnParameters();

	FItemSpawnRequestCompletedEvent RequestCompletedEvent = FItemSpawnRequestCompletedEvent();
	TArray<FLoadingItemData> ItemsToLoad;
};
#pragma endregion

#pragma region ASyncItemData
DECLARE_EVENT_TwoParams(UTartarusItemSubsystem, FGetItemDataRequestCompletedEvent, FGuid /*RequestId*/, TArray<UTartarusItem*> /*ItemsData*/)

USTRUCT()
struct FGetItemDataRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FGetItemDataRequestInfo() {}
	FGetItemDataRequestInfo(const TArray<FPrimaryAssetId>& ItemIdsToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted);
	FGetItemDataRequestInfo(const TArray<EItemType>& ItemTypesToLoad, const FGetItemDataRequestCompletedEvent& OnCompleted);

	const FGetItemDataRequestCompletedEvent& OnGetItemDataRequestCompleted() const { return RequestCompletedEvent; }
	const TArray<FPrimaryAssetId>& GetItemIds() { return ItemIds; }
	const TArray<EItemType>& GetItemTypes() { return ItemTypes; }

private:
	FGetItemDataRequestCompletedEvent RequestCompletedEvent = FGetItemDataRequestCompletedEvent();
	TArray<FPrimaryAssetId> ItemIds;
	TArray<EItemType> ItemTypes;
};
#pragma endregion
