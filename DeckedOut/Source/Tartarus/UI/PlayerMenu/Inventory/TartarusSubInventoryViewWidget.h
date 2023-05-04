// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Inventory/TartarusinventoryData.h"
#include "System/TartarusASyncLoadData.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusSubInventoryViewWidget.generated.h"

class UCommonTileView;
class UTartarusContextAction;
class UTartarusInventoryComponent;
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
class TARTARUS_API UTartarusSubInventoryViewWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
public:
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

#pragma region Inventory
public:
	void LinkInventory(UTartarusInventoryComponent* const Inventory, const EInventoryType SubInventoryId);
	EInventoryType GetInventoryId() const { return InventoryId; }
protected:
	/*
	* Retries all entries that are represented in this View.
	* Return: EnventoryStack entries of the representing Sub-Inventory.
	*/
	const TArray<FInventoryStack>* GetInventoryEntries() const;

	void OnInventoryUpdated(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize);
	void OnInventoryRefreshed();

private:
	TWeakObjectPtr<UTartarusInventoryComponent> InventoryComponent = nullptr;
	EInventoryType InventoryId = EInventoryType::MAX;
#pragma endregion
};
