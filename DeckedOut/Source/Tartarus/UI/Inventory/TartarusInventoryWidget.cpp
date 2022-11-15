// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventoryWidget.h"

#include "CommonTileView.h"
#include "Input/CommonUIInputTypes.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "System/TartarusAssetManager.h"
#include "UI/Inventory/TartarusInventorySlotWidget.h"

void UTartarusInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
	ConstructInventoryView();
}

void UTartarusInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	UpdateSlotDisplays();
}

void UTartarusInventoryWidget::ConstructInventoryView()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No player controller!"), __FUNCTION__);
		return;
	}

	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No inventory found!"), __FUNCTION__);
		return;
	}

	const TArray<FInventoryItemStack>& InventorySlots = InventoryComponent->GetOverview();

	// For each slot in the inventory, create a SlotWidget.
	for (const FInventoryItemStack& InventorySlot : InventorySlots)
	{
		UTartarusInventorySlotWidget* const SlotWidget = CreateWidget<UTartarusInventorySlotWidget>(GetWorld(), TileView->GetEntryWidgetClass());

		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of Inventory Tile view!"), __FUNCTION__);
			continue;
		}

		TileView->AddItem(SlotWidget);
	}
}

void UTartarusInventoryWidget::UpdateSlotDisplays()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No player controller!"), __FUNCTION__);
		return;
	}
	
	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No inventory found!"), __FUNCTION__);
		return;
	}
	
	const TArray<FInventoryItemStack>& InventorySlots = InventoryComponent->GetOverview();

	// For each inventory slot, update the texture to the content.
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		UTartarusInventorySlotWidget* const SlotWidget = Cast<UTartarusInventorySlotWidget>(TileView->GetItemAt(i));

		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No tile found!"), __FUNCTION__);
			continue;
		}

		// Create an async request to load the appropriate texture.
		FUpdateInventoryUIRequestCompletedEvent OnRequestCompleted;

		AsyncRequestSetDisplayTexture(SlotWidget, InventorySlots[i].GetItemId(), OnRequestCompleted);
	}
}

#pragma region BoundActions
void UTartarusInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SelectBoundActionArguments = FBindUIActionArgs(SelectInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectAction));
	SelectBoundActionArguments.bDisplayInActionBar = true;

	SelectActionHandle = RegisterUIActionBinding(SelectBoundActionArguments);

	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UTartarusInventoryWidget::HandleSelectAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}

void UTartarusInventoryWidget::HandleSortAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}
#pragma endregion

#pragma region ASyncLoading
FGuid UTartarusInventoryWidget::AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidget* const SlotWidget, const int32 ItemId, FUpdateInventoryUIRequestCompletedEvent& OnRequestCompletedEvent)
{
	// Get the ItemSubsystem.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: ItemSubsystem was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Create a request to handle updating the UI.
	FUpdateInventoryUIRequestInfo UpdateRequest = FUpdateInventoryUIRequestInfo(OnRequestCompletedEvent, ItemId, SlotWidget);

	// Prepare a callback for when the itemsubsystem has loaded the items their data.
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusInventoryWidget::HandleItemsDataLoaded);

	// Create a request to load the data/
	TArray<int32> ItemIds;
	ItemIds.Add(ItemId);

	FGuid AsyncLoadRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
		
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}
	
	UpdateRequest.SetASyncLoadRequestId(AsyncLoadRequestId);
	UpdateUIRequests.Add(UpdateRequest);
	
	return UpdateRequest.GetRequestId();
}

void UTartarusInventoryWidget::HandleRequestSuccess(const FUpdateInventoryUIRequestInfo* const SuccessRequest, TWeakObjectPtr<UTexture2D> Texture)
{
	if (!SuccessRequest)
	{
		return;
	}

	SuccessRequest->OnUpdateUIRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), Texture);
	UpdateUIRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusInventoryWidget::HandleRequestFailed(const FUpdateInventoryUIRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->GetWidget().Get()->SetDisplayTexture(nullptr);

	FailedRequest->OnUpdateUIRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	UpdateUIRequests.RemoveSingleSwap(*FailedRequest);
}

// TODO: this whiole method expect only 1 item to be loaded!
void UTartarusInventoryWidget::HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData)
{
	// Get the request that is being handled.
	FUpdateInventoryUIRequestInfo* const CurrentRequest = UpdateUIRequests.FindByPredicate([&ASyncLoadRequestId](const FUpdateInventoryUIRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	// Verify that the correct item is loaded.
	if (ItemsData.IsEmpty() || ItemsData[0].UniqueItemId != CurrentRequest->GetItemId())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: No items loaded or the wrong item got loaded.!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	// Find the row for the item to display.
	const FItemTableRow& ItemRow = ItemsData[0];

	// Request to load the texture.
	 FGuid AsyncLoadRequestId = AsyncRequestLoadTexture(ItemRow);

	 if (!AsyncLoadRequestId.IsValid())
	 {
		 UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		 HandleRequestFailed(CurrentRequest);

		 return;
	 }

	 CurrentRequest->SetASyncLoadRequestId(AsyncLoadRequestId);
}

FGuid UTartarusInventoryWidget::AsyncRequestLoadTexture(const FItemTableRow& ItemDefinition)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to load texture: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Create a callback for when the item table is loaded.
	FAsyncLoadAssetRequestCompletedEvent OnTextureLoaded;
	OnTextureLoaded.AddUObject(this, &UTartarusInventoryWidget::HandleTextureLoaded);

	// Create a request to start loading the Item Table.
	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(ItemDefinition.DisplayTexture.ToSoftObjectPath(), OnTextureLoaded);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to load texture: could not start loading texture!"), __FUNCTION__);
		return FGuid();
	}

	return AsyncLoadRequestId;
}

void UTartarusInventoryWidget::HandleTextureLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FUpdateInventoryUIRequestInfo* const CurrentRequest = UpdateUIRequests.FindByPredicate([&ASyncLoadRequestId](const FUpdateInventoryUIRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Could not find the request!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);
		
		return;
	}

	if (!IsValid(CurrentRequest->GetWidget().Get()))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Widget was invalid!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);
		
		return;
	}

	UTexture2D* const Texture = Cast<UTexture2D>(AssetHandle.Get()->GetLoadedAsset());

	if (!IsValid(Texture))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Set display texture failed: Texture was invalid!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);
		
		return;
	}

	CurrentRequest->GetWidget().Get()->SetDisplayTexture(Texture);
	HandleRequestSuccess(CurrentRequest, Texture);
}

#pragma endregion
