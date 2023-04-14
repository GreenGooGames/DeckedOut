// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusSubInventoryView.h"

#include "CommonTileView.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "System/TartarusAssetManager.h"
#include "UI/Inventory/TartarusInventorySlotWidget.h"

void UTartarusSubInventoryView::LinkInventory(const EInventoryType SubInventoryId)
{
	InventoryId = SubInventoryId;
}

void UTartarusSubInventoryView::NativeOnActivated()
{
	Super::NativeOnActivated();

	// Cronstrcut tiles if there are none present.
	if (TileView->GetNumItems() <= 0)
	{
		ConstructTiles();
	}

	// Update all tiles to reflect the linked inventory.
	UpdateSlots();
}

void UTartarusSubInventoryView::ConstructTiles()
{
	const TArray<FInventoryStack>& InventoryEntries = *GetInventoryEntries();

	// Create a slot for each Entry in the SubInventory.
	for (const FInventoryStack& Stack : InventoryEntries)
	{
		UTartarusInventorySlotWidget* const SlotWidget = CreateWidget<UTartarusInventorySlotWidget>(GetWorld(), TileView->GetEntryWidgetClass());
		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of Inventory Tile view!"), *FString(__FUNCTION__));
			continue;
		}
	
		TileView->AddItem(SlotWidget);
	}
}

void UTartarusSubInventoryView::UpdateSlots()
{
	const TArray<FInventoryStack>& InventoryEntries = *GetInventoryEntries();

	if (TileView->GetNumItems() != InventoryEntries.Num())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Size mismatch between Number of tiles and inventory entries! Did you forget to adjust the UI to the new inventory size?"), *FString(__FUNCTION__));
	}

	for (int32 i = 0; i < InventoryEntries.Num(); i++)
	{
		UTartarusInventorySlotWidget* const SlotWidget = Cast<UTartarusInventorySlotWidget>(TileView->GetItemAt(i));
		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No tile found!"), *FString(__FUNCTION__));
			continue;
		}
	
		// Create an async request to load the appropriate texture.
		FUpdateInventoryUIRequestCompletedEvent OnRequestCompleted;
	
		AsyncRequestSetDisplayTexture(SlotWidget, InventoryEntries[i].GetEntryId(), OnRequestCompleted);
	}
}

const TArray<FInventoryStack>* UTartarusSubInventoryView::GetInventoryEntries() const
{
	// Get the contents of the subinventory this View belongs to.
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct Inventory Tiles failed: No player controller!"), *FString(__FUNCTION__));
		return nullptr;
	}

	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct Inventory Tiles failed: No inventory found!"), *FString(__FUNCTION__));
		return nullptr;
	}

	const TArray<FInventoryStack>& InventoryEntries = InventoryComponent->GetOverview(InventoryId);

	return &InventoryEntries;
}

#pragma region ASyncLoading
FGuid UTartarusSubInventoryView::AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidget* const SlotWidget, const FPrimaryAssetId ItemId, FUpdateInventoryUIRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the ItemSubsystem.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: ItemSubsystem was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a request to handle updating the UI.
	FUpdateInventoryUIRequestInfo UpdateRequest = FUpdateInventoryUIRequestInfo(OnRequestCompletedEvent, ItemId, SlotWidget);
	
	// Prepare a callback for when the itemsubsystem has loaded the items their data.
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusSubInventoryView::HandleItemsDataLoaded);
	
	// Create a request to load the data.
	TArray<FPrimaryAssetId> ItemIds;
	ItemIds.Add(ItemId);
	
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

void UTartarusSubInventoryView::HandleRequestSuccess(const FUpdateInventoryUIRequestInfo* const SuccessRequest, TWeakObjectPtr<UTexture2D> Texture)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnUpdateUIRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), Texture);
	UpdateUIRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusSubInventoryView::HandleRequestFailed(const FUpdateInventoryUIRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->GetWidget().Get()->SetDisplayTexture(nullptr);

	FailedRequest->OnUpdateUIRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	UpdateUIRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusSubInventoryView::HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FUpdateInventoryUIRequestInfo* const CurrentRequest = UpdateUIRequests.FindByPredicate([&ASyncLoadRequestId](const FUpdateInventoryUIRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Could not find the request!"), *FString(__FUNCTION__));
		return;
	}

	// Verify that the correct item is loaded.
	if (ItemsData.IsEmpty() || ItemsData[0]->GetPrimaryAssetId() != CurrentRequest->GetItemId())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: No items loaded or the wrong item got loaded.!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);
	
		return;
	}
	
	// Find the row for the item to display.
	const UTartarusItem* const ItemRow = ItemsData[0];
	
	// Request to load the texture.
	FGuid AsyncLoadRequestId = AsyncRequestLoadTexture(ItemRow);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);
	
		return;
	}
	
	CurrentRequest->SetASyncLoadRequestId(AsyncLoadRequestId);
}

FGuid UTartarusSubInventoryView::AsyncRequestLoadTexture(const UTartarusItem* const ItemDefinition)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to load texture: Asset Manager was invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Create a callback for when the item table is loaded.
	FAsyncLoadAssetRequestCompletedEvent OnTextureLoaded;
	OnTextureLoaded.AddUObject(this, &UTartarusSubInventoryView::HandleTextureLoaded);

	// Create a request to start loading the Item Table.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDefinition->DisplayTexture.ToSoftObjectPath(), OnTextureLoaded);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to load texture: could not start loading texture!"), *FString(__FUNCTION__));
		return FGuid();
	}

	return AsyncLoadRequestId;
}

void UTartarusSubInventoryView::HandleTextureLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FUpdateInventoryUIRequestInfo* const CurrentRequest = UpdateUIRequests.FindByPredicate([&ASyncLoadRequestId](const FUpdateInventoryUIRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Could not find the request!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	if (!IsValid(CurrentRequest->GetWidget().Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Widget was invalid!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	UTexture2D* const Texture = Cast<UTexture2D>(AssetHandle.Get()->GetLoadedAsset());
	if (!IsValid(Texture))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Texture was invalid!"), *FString(__FUNCTION__));
		HandleRequestFailed(CurrentRequest);

		return;
	}

	CurrentRequest->GetWidget().Get()->SetDisplayTexture(Texture);
	HandleRequestSuccess(CurrentRequest, Texture);
}
#pragma endregion