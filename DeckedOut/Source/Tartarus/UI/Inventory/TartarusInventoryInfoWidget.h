// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "System/TartarusHelpers.h"
#include "System/TartarusAsyncLoadData.h"

#include "TartarusInventoryInfoWidget.generated.h"

class UCommonTextBlock;
class UTartarusItem;

#pragma region ASyncLoading
USTRUCT()
struct FItemInfoRequest : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FItemInfoRequest() {}
	FItemInfoRequest(const FPrimaryAssetId& ToRepresentItemId);

	const FPrimaryAssetId& GetItemId() const { return ItemId; }

private:
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventoryInfoWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void SetItemReference(const FPrimaryAssetId& ToRepresentItemId);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> NameTextBlock = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> DescriptionTextBlock = nullptr;

private:
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;

#pragma region ASyncLoading
protected:
	FGuid AsyncRequestItemData(const FPrimaryAssetId& ToRepresentItemId);
	void HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleRequestCompleted(const FItemInfoRequest* const CompletedRequest, const UTartarusItem* const RepresentingItem);

private:
	TArray<FItemInfoRequest> ItemInfoRequests;
#pragma endregion
};
