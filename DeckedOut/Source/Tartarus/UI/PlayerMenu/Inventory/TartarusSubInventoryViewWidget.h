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
USTRUCT()
struct FUpdateInventoryUIRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FUpdateInventoryUIRequestInfo() {}
	FUpdateInventoryUIRequestInfo(TArray<UTartarusInventorySlotWidgetData*> SlotsData);

	const TArray<UTartarusInventorySlotWidgetData*>& GetSlotWidgetsData() const { return SlotWidgetsData; }

private:
	TArray<UTartarusInventorySlotWidgetData*> SlotWidgetsData;
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
	UCommonTileView* GetTileView() const;

protected:
	/*
	* TileView holding all display entries that represent the Entry stored in the inventory.
	*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTileView> TileView = nullptr;

	/*
	* Refreshes the existing data and adds/removes data if necesary.
	*/
	void RefreshData();

	/*
	* Creates a new ListItem entry for the TileView.
	*/
	UTartarusInventorySlotWidgetData* CreateListItemData();

#pragma region UCommonActivatableWidget
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion

#pragma region ASyncLoading
protected:
	/*
	* Creates a request to update the texture of a slot for a given item.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestSetDisplayTexture(TArray<UTartarusInventorySlotWidgetData*> SlotsData);

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

	void OnInventoryEntryUpdated(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize);

private:
	TWeakObjectPtr<UTartarusInventoryComponent> InventoryComponent = nullptr;
	EInventoryType InventoryId = EInventoryType::MAX;
#pragma endregion
};
