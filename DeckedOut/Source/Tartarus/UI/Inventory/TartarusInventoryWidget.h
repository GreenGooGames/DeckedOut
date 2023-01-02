// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/TartarusASyncLoadData.h"
#include "System/TartarusHelpers.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusInventoryWidget.generated.h"

class UCommonTileView;
class UTartarusInventorySlotWidget;

struct FStreamableHandle;
struct FItemTableRow;

DECLARE_EVENT_TwoParams(UTartarusInventoryWidget, FUpdateInventoryUIRequestCompletedEvent, FGuid /*RequestId*/, TWeakObjectPtr<UTexture2D> /*DisplayTexture*/)

USTRUCT()
struct FUpdateInventoryUIRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FUpdateInventoryUIRequestInfo() {}
	FUpdateInventoryUIRequestInfo(const FUpdateInventoryUIRequestCompletedEvent& OnCompleted, const int32 ItemToLoadId, UTartarusInventorySlotWidget* const Widget)
	{
		RequestId = FGuid::NewGuid();

		OnRequestCompleteEvent = OnCompleted;
		ItemId = ItemToLoadId;
		SlotWidget = Widget;
	}

	const FUpdateInventoryUIRequestCompletedEvent& OnUpdateUIRequestCompleted() const { return OnRequestCompleteEvent; }
	int32 GetItemId() const { return ItemId; }
	TWeakObjectPtr<UTartarusInventorySlotWidget> GetWidget() const { return SlotWidget; }

private:
	FUpdateInventoryUIRequestCompletedEvent OnRequestCompleteEvent = FUpdateInventoryUIRequestCompletedEvent();

	int32 ItemId = FTartarusHelpers::InvalidItemId;
	TWeakObjectPtr<UTartarusInventorySlotWidget> SlotWidget = nullptr;
};


/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventoryWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTileView> TileView = nullptr;

	void ConstructInventoryView();
	void UpdateSlotDisplays();

#pragma region BoundActions
protected:
	// Input action to select the current focused item.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
		FDataTableRowHandle SelectInputActionData;

	// Input action to sort the Inventory.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
		FDataTableRowHandle SortInputActionData;

private:
	FUIActionBindingHandle SelectActionHandle;
	FUIActionBindingHandle SortActionHandle;

	void RegisterBoundInputActions();
	void HandleSelectAction();
	void HandleSortAction();
#pragma endregion

#pragma region ASyncLoading
protected:
	/*
	* Creates a request to update the texture of a slot for a given item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidget* const SlotWidget, const int32 ItemId, FUpdateInventoryUIRequestCompletedEvent& OnRequestCompletedEvent);

	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FUpdateInventoryUIRequestInfo* const SuccessRequest, TWeakObjectPtr<UTexture2D> Texture);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FUpdateInventoryUIRequestInfo* const FailedRequest);

	// Called when the items their data is loaded.
	void HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData);

	/*
	* Creates a request to load a texture.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestLoadTexture(const FItemTableRow& ItemDefinitionn);

	// Called when the Item UI Display Texture is loaded.
	void HandleTextureLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

private:
	TArray<FUpdateInventoryUIRequestInfo> UpdateUIRequests;
#pragma endregion
};
