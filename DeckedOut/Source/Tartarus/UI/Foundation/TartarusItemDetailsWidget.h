// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"
#include "System/TartarusAsyncLoadData.h"

#include "TartarusItemDetailsWidget.generated.h"

class UCommonLazyImage;
class UCommonListView;
class UTartarusItem;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemDetailsWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
public:
	void SetItemReference(const FPrimaryAssetId& ToRepresentItemId);

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> ItemImage = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonListView> ItemInfoListView = nullptr;

	void Refresh(UTartarusItem* const Item);

private:
	FPrimaryAssetId RepresentingItemId = FPrimaryAssetId();

#pragma region AsyncLoading
protected:
	void ASyncRequestLoadItemData();
	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleOnRequestCompleted(FASyncLoadRequest* const CompletedRequest);

private:
	TArray<FASyncLoadRequest> ItemDataRequests;
#pragma endregion
};
