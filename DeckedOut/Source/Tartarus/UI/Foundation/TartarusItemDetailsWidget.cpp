// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusItemDetailsWidget.h"
#include "CommonListView.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Logging/TartarusLogChannels.h"
#include "Item/TartarusItem.h"
#include "CommonLazyImage.h"
#include "UI/Foundation/TartarusItemInfoEntryWidgetData.h"
#include "Item/TartarusItem.h"

void UTartarusItemDetailsWidget::SetItemReference(const FPrimaryAssetId& ToRepresentItemId)
{
	if (RepresentingItemId == ToRepresentItemId)
	{
		return;
	}

	RepresentingItemId = ToRepresentItemId;

	ASyncRequestLoadItemData();
}

void UTartarusItemDetailsWidget::Refresh(UTartarusItem* const Item)
{
	ItemImage->SetBrushFromLazyTexture(Item->DisplayTexture);

	// TODO: If an entry exist overwrite it.
	ItemInfoListView->ClearListItems();

	// TODO: HAha ugly pls refactor
	// Add an entry item to the Listview for each elements to display.
	UTartarusItemInfoEntryWidgetData* const NameEntryData = NewObject<UTartarusItemInfoEntryWidgetData>();
	NameEntryData->MainText = FText::FromString("Name");
	NameEntryData->SubText = Item->Name;
	ItemInfoListView->AddItem(NameEntryData);

	UTartarusItemInfoEntryWidgetData* const DescriptionEntryData = NewObject<UTartarusItemInfoEntryWidgetData>();
	DescriptionEntryData->MainText = FText::FromString("Description");
	DescriptionEntryData->SubText = Item->Description;
	ItemInfoListView->AddItem(DescriptionEntryData);
}

#pragma region AsyncLoading
void UTartarusItemDetailsWidget::ASyncRequestLoadItemData()
{
	// Get the ItemSubsystem.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to request ItemData: ItemSubsystem was invalid!"), *FString(__FUNCTION__));
		return;
	}

	// Prepare a callback for when the ItemSubsystem has loaded the requested item data.
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusItemDetailsWidget::HandleItemDataLoaded);

	// Create a request to load the data.
	const TArray<FPrimaryAssetId> ItemIds = { RepresentingItemId };

	FGuid AsyncLoadRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to request ItemData: Could not start async load!"), *FString(__FUNCTION__));
		return;
	}

	FASyncLoadRequest Request = FASyncLoadRequest();
	Request.SetASyncLoadRequestId(AsyncLoadRequestId);
	ItemDataRequests.Add(Request);
}

void UTartarusItemDetailsWidget::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FASyncLoadRequest* const CurrentRequest = ItemDataRequests.FindByPredicate([&ASyncLoadRequestId](const FASyncLoadRequest& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to handle the loaded item data: Could not find the request!"), *FString(__FUNCTION__));
		HandleOnRequestCompleted(CurrentRequest);
		return;
	}

	UTartarusItem* const LoadedItem = ItemsData.IsEmpty() ? nullptr : ItemsData[0];
	if (!IsValid(LoadedItem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to handle the loaded item data: the Item is null!"), *FString(__FUNCTION__));
		HandleOnRequestCompleted(CurrentRequest);
		return;
	}

	Refresh(LoadedItem);

	HandleOnRequestCompleted(CurrentRequest);
}

void UTartarusItemDetailsWidget::HandleOnRequestCompleted(FASyncLoadRequest* const CompletedRequest)
{
	if (!CompletedRequest)
	{
		return;
	}

	ItemDataRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma endregion
