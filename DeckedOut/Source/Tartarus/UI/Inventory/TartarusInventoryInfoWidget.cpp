// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventoryInfoWidget.h"

#include "CommonTextBlock.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"

void UTartarusInventoryInfoWidget::SetItemReference(const FPrimaryAssetId& ToRepresentItemId)
{
	ItemId = ToRepresentItemId;

	AsyncRequestItemData(ItemId);
}

#pragma region ASyncLoading
FItemInfoRequest::FItemInfoRequest(const FPrimaryAssetId& ToRepresentItemId)
{
	RequestId = FGuid::NewGuid();

	ItemId = ToRepresentItemId;
}

FGuid UTartarusInventoryInfoWidget::AsyncRequestItemData(const FPrimaryAssetId& ToRepresentItemId)
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
	OnRequestCompleted.AddUObject(this, &UTartarusInventoryInfoWidget::HandleItemsDataLoaded);

	// Create a request to load the data.
	TArray<FPrimaryAssetId> ItemIds;
	ItemIds.Add(ToRepresentItemId);

	FGuid AsyncLoadRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FItemInfoRequest Request = FItemInfoRequest(ToRepresentItemId);
	Request.SetASyncLoadRequestId(AsyncLoadRequestId);
	ItemInfoRequests.Add(Request);

	return Request.GetRequestId();
}

void UTartarusInventoryInfoWidget::HandleItemsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FItemInfoRequest* const CurrentRequest = ItemInfoRequests.FindByPredicate([&ASyncLoadRequestId](const FItemInfoRequest& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Display Hovered Item info Failed: Could not find the request!"), *FString(__FUNCTION__));
		HandleRequestCompleted(CurrentRequest, nullptr);
		return;
	}

	if (CurrentRequest->GetItemId() != ItemId)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Display Hovered Item info Failed: Need to represent another item!"), *FString(__FUNCTION__));
		HandleRequestCompleted(CurrentRequest, nullptr);
		return;
	}

	const UTartarusItem* const LoadedItem = ItemsData.IsEmpty() ? nullptr : ItemsData[0];
	if (!IsValid(LoadedItem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Display Hovered Item info Failed: the Item is null!"), *FString(__FUNCTION__));
		HandleRequestCompleted(CurrentRequest, nullptr);
		return;
	}

	HandleRequestCompleted(CurrentRequest, LoadedItem);
}

void UTartarusInventoryInfoWidget::HandleRequestCompleted(const FItemInfoRequest* const CompletedRequest, const UTartarusItem* const RepresentingItem)
{
	if (!CompletedRequest)
	{
		return;
	}

	if (IsValid(RepresentingItem))
	{
		ActivateWidget();

		NameTextBlock->SetText(RepresentingItem->Name);
		DescriptionTextBlock->SetText(RepresentingItem->Description);
	}
	else
	{
		DeactivateWidget();

		NameTextBlock->SetText(FText::FromString(""));
		DescriptionTextBlock->SetText(FText::FromString(""));
	}

	ItemInfoRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma endregion
