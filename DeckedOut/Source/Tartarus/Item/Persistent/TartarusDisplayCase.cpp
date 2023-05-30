// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Persistent/TartarusDisplayCase.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Item/TartarusItemInstance.h"
#include "Logging/TartarusLogChannels.h"

#pragma region FDisplayCaseSlot
FDisplayCaseSlot::FDisplayCaseSlot(const FVector& SlotLocation)
{
	Location = SlotLocation;
}

bool FDisplayCaseSlot::IsAvailable() const
{
	return !InventoryStackId.IsValid();
}

void FDisplayCaseSlot::SetDisplayedItem(const FInventoryStackId& ItemInventoryStackId, ATartarusItemInstance* const ToDisplay)
{
	InventoryStackId = ItemInventoryStackId;
	Item = ToDisplay;
}
#pragma endregion

// Sets default values
ATartarusDisplayCase::ATartarusDisplayCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InventoryComponent = CreateDefaultSubobject<UTartarusInventoryComponent>("Inventory component", false);
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	InventoryComponent->OnInventoryEntryUpdated().AddUObject(this, &ATartarusDisplayCase::OnInventoryEntryChanged);
}

#pragma region Display
void ATartarusDisplayCase::UpdateDisplaySlots()
{
	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to Update the display Slots: The inventory is invalid!"), *FString(__FUNCTION__));
		return;
	}

	const TArray<FInventoryStack>& InventoryEntries = InventoryComponent->GetOverview(InventoryComponent->GetSubInventoryIds()[0]);

	if (DisplaySlots.Num() != InventoryEntries.Num())
	{
		DisplaySlots.SetNum(InventoryEntries.Num());
	}

	for (int32 i = 0; i < InventoryEntries.Num(); i++)
	{
		// Skip this InventoryEntry if it is already being displayed in the corresponding slot.
		if (DisplaySlots[i].GetInventoryStackId() == InventoryEntries[i].GetStackId())
		{
			continue;
		}

		// If there is no valid entry, nothings needs to be displayed.
		if (!InventoryEntries[i].GetEntryId().IsValid())
		{
			// If an item is currently being shown, remove it.
			if (IsValid(DisplaySlots[i].GetDisplayItem().Get()))
			{
				RemoveItem(DisplaySlots[i]);
			}

			continue;
		}

		// This entry is not being displayed, make a request to display it.
		DisplayItemASync(DisplaySlots[i], InventoryEntries[i]);
	}
}

void ATartarusDisplayCase::DisplayItemASync(FDisplayCaseSlot& Slot, const FInventoryStack& InventoryStack)
{
	FDisplayRequestInfo RequestInfo = FDisplayRequestInfo(&Slot, &InventoryStack);

	const TArray<FPrimaryAssetId> AssetsToLoad = { InventoryStack.GetEntryId() };
	const FGuid RequestId = RequestItemDataAsync(AssetsToLoad);
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to display an item: Could not create an ASync request!"), *FString(__FUNCTION__));
		return;
	}

	RequestInfo.SetASyncLoadRequestId(RequestId);
	DisplayRequests.Add(RequestInfo);
}

void ATartarusDisplayCase::DisplayItem(FDisplayCaseSlot& Slot, const FInventoryStack& InventoryStack, ATartarusItemInstance* const ItemInstance)
{
	if (!IsValid(ItemInstance))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: The ItemInstance is invalid!"), *FString(__FUNCTION__));
		return;
	}

	// Attach the Instance to this object.
	Slot.SetDisplayedItem(InventoryStack.GetStackId(), ItemInstance);

	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;

	UPrimitiveComponent* PhysicsComponent = Cast<UPrimitiveComponent>(ItemInstance->GetRootComponent());
	PhysicsComponent->SetSimulatePhysics(false);
	ItemInstance->AttachToActor(this, AttachmentRules);

	const FVector RelativeLocation = Slot.GetSlotRelativeLocation();
	ItemInstance->SetActorRelativeLocation(RelativeLocation);
}

void ATartarusDisplayCase::RemoveItem(FDisplayCaseSlot& Slot)
{
	ATartarusItemInstance* const DisplayInstance = Slot.GetDisplayItem().Get();

	// Get the ItemSubsystem to despawn the display item.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return;
	}

	const bool bIsDespawned = ItemSubsystem->DespawnItem(DisplayInstance);
	if (!bIsDespawned)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item not despawned!"), *FString(__FUNCTION__));
		return;
	}

	// Clear the DisplaySlots entry,
	Slot.SetDisplayedItem(FInventoryStackId(), nullptr);
}
#pragma endregion

#pragma region Inventory
void ATartarusDisplayCase::OnInventoryEntryChanged(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize)
{
	UpdateDisplaySlots();
}
#pragma endregion

#pragma region AsyncLoad
#pragma optimize("", off)
ATartarusDisplayCase::FDisplayRequestInfo::FDisplayRequestInfo(FDisplayCaseSlot* const Slot, const FInventoryStack* ToDisplayStack)
{
	DisplaySlot = Slot;
	InventoryStack = ToDisplayStack;
}

FGuid ATartarusDisplayCase::RequestItemDataAsync(TArray<FPrimaryAssetId> AssetsToLoad)
{
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create a request to retrieve the Item data: ItemSubsystem is not found!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusDisplayCase::HandleItemDataLoaded);

	const FGuid RequestId = ItemSubsystem->AsyncRequestGetItemsData(AssetsToLoad, OnRequestCompleted);
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create a request to retrieve the Item data: Failed to create a request!"), *FString(__FUNCTION__));
		return FGuid();
	}

	return RequestId;
}

void ATartarusDisplayCase::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FDisplayRequestInfo* const CurrentRequest = DisplayRequests.FindByPredicate([&ASyncLoadRequestId](const FDisplayRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});
	if (!CurrentRequest)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	// Get the item subsystem.
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: World is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	// Make a request to spawn the Item.
	FItemSpawnRequestCompletedEvent OnItemsSpawned;
	OnItemsSpawned.AddUObject(this, &ATartarusDisplayCase::HandleItemSpawned);

	FGuid ASyncRequestId = ItemSubsystem->AsyncRequestSpawnItems(ItemsData, FTransform::Identity, ItemSpawnParameters, OnItemsSpawned);
	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not request the item data!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	CurrentRequest->SetASyncLoadRequestId(ASyncRequestId);
}

void ATartarusDisplayCase::HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedItems)
{
	// Get the request that is being handled.
	FDisplayRequestInfo* const CurrentRequest = DisplayRequests.FindByPredicate([&ASyncLoadRequestId](const FDisplayRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});
	if (!CurrentRequest)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	if (CurrentRequest->GetDisplaySlot() == nullptr)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: The DisplaySlot is no longer valid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	if (CurrentRequest->GetInventoryStack() == nullptr)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: The InventoryStack is no longer valid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	DisplayItem(*CurrentRequest->GetDisplaySlot(), *CurrentRequest->GetInventoryStack(), SpawnedItems[0].Get());

	HandleRequestCompleted(CurrentRequest);
}

void ATartarusDisplayCase::HandleRequestCompleted(const FDisplayRequestInfo* const CompletedRequest)
{
	if (!CompletedRequest)
	{
		return;
	}

	DisplayRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma optimize("", on)
#pragma endregion
