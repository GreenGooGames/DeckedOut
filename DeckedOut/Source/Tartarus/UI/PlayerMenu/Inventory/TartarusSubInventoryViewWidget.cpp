// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerMenu/Inventory/TartarusSubInventoryViewWidget.h"

#include "CommonTileView.h"
#include "Engine/Texture2D.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"

#pragma region FUpdateInventoryUIRequestInfo
FUpdateInventoryUIRequestInfo::FUpdateInventoryUIRequestInfo(TArray<UTartarusInventorySlotWidgetData*> SlotsData)
{
	RequestId = FGuid::NewGuid();

	SlotWidgetsData = SlotsData;
}
#pragma endregion

UCommonTileView* UTartarusSubInventoryViewWidget::GetTileView() const
{
	return TileView;
}

void UTartarusSubInventoryViewWidget::RefreshData()
{
	TArray<UTartarusInventorySlotWidgetData*> SlotsData;

	// Get all existing ListItems and all Inventory Entries.
	const TArray<UObject*> ListItems = TileView->GetListItems();
	const TArray<FInventoryStack>& InventoryEntries = *GetInventoryEntries();
	for (int i = 0; i < InventoryEntries.Num(); i++)
	{
		// If a ListItem exist to assign to the InventoryItem, retrieve and update. Else Create a new ListItem and then set the data.
		UTartarusInventorySlotWidgetData* const ListItem = ListItems.Num() > i ? Cast<UTartarusInventorySlotWidgetData>(ListItems[i]) : CreateListItemData();
		if (!IsValid(ListItem))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Unable to refresh data: Could not retrieve or create a list entry!"), *FString(__FUNCTION__));
			continue;
		}

		ListItem->UpdateData(&InventoryEntries[i]);
		SlotsData.Add(ListItem);
	}

	// Create an async request link the appropriate texture.
	AsyncRequestSetDisplayTexture(SlotsData);
}

UTartarusInventorySlotWidgetData* UTartarusSubInventoryViewWidget::CreateListItemData()
{
	// Create an instance to save all the data in.
	UTartarusInventorySlotWidgetData* const SlotData = NewObject<UTartarusInventorySlotWidgetData>(this);
	if (!IsValid(SlotData))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of InventorySlotWidgetData!"), *FString(__FUNCTION__));
		return nullptr;
	}

	// Add the data to the Tileview to display.
	TileView->AddItem(SlotData);

	return SlotData;
}

#pragma region UCommonActivatableWidget
void UTartarusSubInventoryViewWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!IsValid(InventoryComponent.Get()))
	{
		return;
	}

	InventoryComponent->OnInventoryEntryUpdated().AddUObject(this, &UTartarusSubInventoryViewWidget::OnInventoryEntryUpdated);
	InventoryComponent->OnInventoryContentUpdated().AddUObject(this, &UTartarusSubInventoryViewWidget::RefreshData);

	// Create entries for each item in the inventory or refresh the existing ones.
	RefreshData();
}

void UTartarusSubInventoryViewWidget::NativeOnDeactivated()
{
	if (!IsValid(InventoryComponent.Get()))
	{
		return;
	}

	InventoryComponent->OnInventoryEntryUpdated().RemoveAll(this);
	InventoryComponent->OnInventoryContentUpdated().RemoveAll(this);
}

UWidget* UTartarusSubInventoryViewWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(TileView))
	{
		return nullptr;
	}

	return TileView;
}
#pragma endregion

#pragma region ASyncLoading
FGuid UTartarusSubInventoryViewWidget::AsyncRequestSetDisplayTexture(TArray<UTartarusInventorySlotWidgetData*> SlotsData)
{
	// Get the ItemSubsystem.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: ItemSubsystem was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}
	
	// Prepare a callback for when the itemsubsystem has loaded the items their data.
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusSubInventoryViewWidget::HandleItemsDataLoaded);
	
	// Gather all Id's to load.
	TArray<FPrimaryAssetId> ItemIds;

	for (UTartarusInventorySlotWidgetData* const SlotData : SlotsData)
	{
		if (!SlotData->GetItemId().IsValid())
		{
			continue;
		}

		ItemIds.Add(SlotData->GetItemId());
	}

	// Make a request to the ItemSubsystem to start loading the Item Data.
	FGuid AsyncLoadRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle updating the UI.
	FUpdateInventoryUIRequestInfo UpdateRequest = FUpdateInventoryUIRequestInfo(SlotsData);
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

	// For each SlotWidget ListEntry that needs to be updated, get the corresponding loaded data.
	for (UTartarusInventorySlotWidgetData* const SlotWidgetData : CurrentRequest->GetSlotWidgetsData())
	{
		// Find the loaded data for the current SlotWidgetData.
		UTartarusItem** ItemDataPtr = ItemsData.FindByPredicate([&SlotWidgetData](UTartarusItem* Item)
			{
				return Item->GetPrimaryAssetId() == SlotWidgetData->GetItemId();
			});

		if (ItemDataPtr == nullptr)
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed for a single entry: Could not find the corresponding loaded data!"), *FString(__FUNCTION__));
			continue;
		}

		const UTartarusItem* const ItemData = *ItemDataPtr;
		if (!IsValid(ItemData))
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed for a single entry: Could not convert the loaded data to a pointer!"), *FString(__FUNCTION__));
			continue;
		}

		// Update the texture of the SlotWidgetData.
		SlotWidgetData->SetTexture(ItemData->DisplayTexture);
	}
	
	HandleRequestCompleted(CurrentRequest);
}
#pragma endregion

#pragma region Inventory
void UTartarusSubInventoryViewWidget::LinkInventory(UTartarusInventoryComponent* const Inventory, const EInventoryType SubInventoryId)
{
	InventoryId = SubInventoryId;
	InventoryComponent = Inventory;

	// Create entries for each item in the inventory or refresh the existing ones.
	RefreshData();
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

void UTartarusSubInventoryViewWidget::OnInventoryEntryUpdated(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize)
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
		
		// If another item is currently occupying the slot in the inventory, update the Display Texture.
		if (InventoryEntry != nullptr && ItemData->GetItemId() != InventoryEntry->GetEntryId())
		{
			TArray<UTartarusInventorySlotWidgetData*> ToLoadItemDataWidgets = {ItemData};
			AsyncRequestSetDisplayTexture(ToLoadItemDataWidgets);
		}

		// Update The data stored to reflect the inventory entry.
		ItemData->UpdateData(InventoryEntry);
	}
}
#pragma endregion
