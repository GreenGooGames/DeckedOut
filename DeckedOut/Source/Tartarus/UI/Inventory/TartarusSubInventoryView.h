// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Inventory/TartarusinventoryData.h"
#include "System/TartarusASyncLoadData.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusSubInventoryView.generated.h"

class UCommonTileView;
class UTartarusContextAction;
class UTartarusInventorySlotWidgetData;
class UTartarusItem;

#pragma region ASyncLoading
DECLARE_EVENT_OneParam(UTartarusInventoryWidget, FUpdateInventoryUIRequestCompletedEvent, FGuid /*RequestId*/)

USTRUCT()
struct FUpdateInventoryUIRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FUpdateInventoryUIRequestInfo() {}
	FUpdateInventoryUIRequestInfo(const FUpdateInventoryUIRequestCompletedEvent& OnCompleted, UTartarusInventorySlotWidgetData* const SlotData);

	const FUpdateInventoryUIRequestCompletedEvent& OnUpdateUIRequestCompleted() const { return OnRequestCompleteEvent; }
	UTartarusInventorySlotWidgetData* GetSlotWidgetData() const { return SlotWidgetData; }

private:
	FUpdateInventoryUIRequestCompletedEvent OnRequestCompleteEvent = FUpdateInventoryUIRequestCompletedEvent();

	UTartarusInventorySlotWidgetData* SlotWidgetData = nullptr;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSubInventoryView : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
public:
	void LinkInventory(const EInventoryType SubInventoryId);
	TArray<UTartarusContextAction*> GetContextActions();

	UCommonTileView* GetTileView() const;

protected:
	virtual void NativeOnActivated() override;

protected:
	/*
	* TileView holding all display entries that represent the Entry stored in the inventory.
	*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTileView> TileView = nullptr;

	/*
	* Add data to be displayed in the TileView.
	*/
	void InitializeData();

	/*
	* Retries all entries that are represented in this View.
	* Return: EnventoryStack entries of the representing Sub-Inventory.
	*/
	const TArray<FInventoryStack>* GetInventoryEntries() const;

private:
	EInventoryType InventoryId = EInventoryType::MAX;

#pragma region ASyncLoading
protected:
	/*
	* Creates a request to update the texture of a slot for a given item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidgetData* const SlotData, FUpdateInventoryUIRequestCompletedEvent& OnRequestCompletedEvent);

	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestCompleted(const FUpdateInventoryUIRequestInfo* const CompletedRequest);

	// Called when the items their data is loaded.
	void HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);

private:
	TArray<FUpdateInventoryUIRequestInfo> UpdateUIRequests;
#pragma endregion
};
