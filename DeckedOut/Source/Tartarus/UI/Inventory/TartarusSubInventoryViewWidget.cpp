// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusSubInventoryViewWidget.h"

#include "CommonTileView.h"
#include "Engine/Texture2D.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/ContextAction/TartarusContextAction.h"
#include "UI/Inventory/TartarusInventorySlotWidgetData.h"

#pragma region FUpdateInventoryUIRequestInfo
FUpdateInventoryUIRequestInfo::FUpdateInventoryUIRequestInfo(const FUpdateInventoryUIRequestCompletedEvent& OnCompleted, UTartarusInventorySlotWidgetData* const SlotData)
{
	RequestId = FGuid::NewGuid();

	OnRequestCompleteEvent = OnCompleted;
	SlotWidgetData = SlotData;
}
#pragma endregion

TArray<UTartarusContextAction*> UTartarusSubInventoryViewWidget::GetContextActions()
{
	TArray<UTartarusContextAction*> ContextActions;

	if (!IsValid(InventoryComponent.Get()))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to retrieve context actions: Inventory is invalid!"), *FString(__FUNCTION__));
		return ContextActions;
	}

	ContextActions = InventoryComponent->GetSubInventoryContextActions(InventoryId);

	return ContextActions;
}

UCommonTileView* UTartarusSubInventoryViewWidget::GetTileView() const
{
	return TileView;
}

void UTartarusSubInventoryViewWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// Clear the old items as the inventory could have changed drastically.
	TileView->ClearListItems();

	// Create entries for each item in the inventory.
	InitializeData();
}

void UTartarusSubInventoryViewWidget::InitializeData()
{
	const TArray<FInventoryStack>& InventoryEntries = *GetInventoryEntries();

	// TODO: Combine all ASync requests into a single request as the request only loads the datatable and then searches for the correct item doing nothing further.
	// For each item in the inventory create a data item.
	for (const FInventoryStack& Stack : InventoryEntries)
	{
		// Create an instance to save all the data in.
		UTartarusInventorySlotWidgetData* const SlotData = NewObject<UTartarusInventorySlotWidgetData>(this);
		if (!IsValid(SlotData))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of InventorySlotWidgetData!"), *FString(__FUNCTION__));
			continue;
		}

		SlotData->UpdateData(&Stack);

		// Create an async request link the appropriate texture.
		FUpdateInventoryUIRequestCompletedEvent OnRequestCompleted;
		AsyncRequestSetDisplayTexture(SlotData, OnRequestCompleted);

		// Add the data to the Tileview to display.
		TileView->AddItem(SlotData);
	}
}

#pragma region ASyncLoading
FGuid UTartarusSubInventoryViewWidget::AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidgetData* const SlotData, FUpdateInventoryUIRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the ItemSubsystem.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: ItemSubsystem was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle updating the UI.
	FUpdateInventoryUIRequestInfo UpdateRequest = FUpdateInventoryUIRequestInfo(OnRequestCompletedEvent, SlotData);
	
	// Prepare a callback for when the itemsubsystem has loaded the items their data.
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusSubInventoryViewWidget::HandleItemsDataLoaded);
	
	// Create a request to load the data.
	TArray<FPrimaryAssetId> ItemIds;
	ItemIds.Add(SlotData->GetItemId());
	
	FGuid AsyncLoadRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	UpdateRequest.SetASyncLoadRequestId(AsyncLoadRequestId);
	UpdateUIRequests.Add(UpdateRequest);
	
	return UpdateRequest.GetRequestId();
}

void UTartarusSubInventoryViewWidget::HandleRequestCompleted(const FUpdateInventoryUIRequestInfo* const CompletedRequest)
{
	if (!CompletedRequest)
	{
		return;
	}

	CompletedRequest->OnUpdateUIRequestCompleted().Broadcast(CompletedRequest->GetRequestId());
	UpdateUIRequests.RemoveSingleSwap(*CompletedRequest);
}

void UTartarusSubInventoryViewWidget::HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FUpdateInventoryUIRequestInfo* const CurrentRequest = UpdateUIRequests.FindByPredicate([&ASyncLoadRequestId](const FUpdateInventoryUIRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Could not find the request!"), *FString(__FUNCTION__));
		HandleRequestCompleted(CurrentRequest);
		return;
	}

	// Verify that the correct item is loaded.
	if (ItemsData.IsEmpty() || ItemsData[0]->GetPrimaryAssetId() != CurrentRequest->GetSlotWidgetData()->GetItemId())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: No items loaded or the wrong item got loaded.!"), *FString(__FUNCTION__));
		HandleRequestCompleted(CurrentRequest);

		return;
	}
	
	// Find the row for the item to display.
	const UTartarusItem* const ItemData = ItemsData[0];
	
	CurrentRequest->GetSlotWidgetData()->SetTexture(ItemData->DisplayTexture);
	HandleRequestCompleted(CurrentRequest);
}
#pragma endregion

#pragma region Inventory
void UTartarusSubInventoryViewWidget::LinkInventory(UTartarusInventoryComponent* const Inventory, const EInventoryType SubInventoryId)
{
	InventoryId = SubInventoryId;
	InventoryComponent = Inventory;

	InventoryComponent->OnInventoryChanged().AddUObject(this, &UTartarusSubInventoryViewWidget::OnInventoryUpdated);
	InventoryComponent->OnInventoryUpdate().AddUObject(this, &UTartarusSubInventoryViewWidget::OnInventoryRefreshed);
}

const TArray<FInventoryStack>* UTartarusSubInventoryViewWidget::GetInventoryEntries() const
{
	if (!IsValid(InventoryComponent.Get()))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct Inventory Tiles failed: No inventory found!"), *FString(__FUNCTION__));
		return nullptr;
	}

	const TArray<FInventoryStack>& InventoryEntries = InventoryComponent->GetOverview(InventoryId);

	return &InventoryEntries;
}

void UTartarusSubInventoryViewWidget::OnInventoryUpdated(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize)
{
	// If this Sub-Inventory contains the InventoryEntry that has changed, update the data for the TileView.
	const TArray<UObject*> ListItems = TileView->GetListItems();

	for (UObject* const Item : ListItems)
	{
		UTartarusInventorySlotWidgetData* const ItemData = Cast<UTartarusInventorySlotWidgetData>(Item);
		if (!IsValid(ItemData))
		{
			continue;
		}

		if (ItemData->GetInventoryStackId() != StackId)
		{
			continue;
		}

		const FInventoryStack* const InventoryEntry = InventoryComponent->GetOverviewSingle(StackId);
		ItemData->UpdateData(InventoryEntry);

		return;
	}
}

void UTartarusSubInventoryViewWidget::OnInventoryRefreshed()
{
	const TArray<UObject*> ListItems = TileView->GetListItems();
	const TArray<FInventoryStack>& InventoryEntries = *GetInventoryEntries();

	if (ListItems.Num() != InventoryEntries.Num())
	{
		return;
	}

	for (int i = 0; i < InventoryEntries.Num(); i++)
	{
		const FInventoryStack& InventoryStack = InventoryEntries[i];
		UTartarusInventorySlotWidgetData* const ListItem = Cast< UTartarusInventorySlotWidgetData>(ListItems[i]);

		ListItem->UpdateData(&InventoryStack);

		// Create an async request link the appropriate texture.
		FUpdateInventoryUIRequestCompletedEvent OnRequestCompleted;
		AsyncRequestSetDisplayTexture(ListItem, OnRequestCompleted);

		// Add the data to the Tileview to display.
		TileView->AddItem(ListItem);
	}
}
#pragma endregion