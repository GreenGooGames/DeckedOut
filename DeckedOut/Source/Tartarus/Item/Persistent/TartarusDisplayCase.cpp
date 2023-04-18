// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Persistent/TartarusDisplayCase.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
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

#pragma region ASyncDisplay
FDisplayRequestInfo::FDisplayRequestInfo(const int32 Slot)
{
	RequestId = FGuid::NewGuid();
	SlotIndex = Slot;
}
#pragma endregion

// Sets default values
ATartarusDisplayCase::ATartarusDisplayCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InventoryComponent = CreateDefaultSubobject<UTartarusInventoryComponent>("Inventory component", false);
}

bool ATartarusDisplayCase::AddToDisplay(const UTartarusItem* const Item)
{
	// Store the item in the inventory.
	// [Koen Goossens] TODO: Magic Number 1.
	const FInventoryStackId InventoryStackId = InventoryComponent->StoreEntry(Item, 1);
	if (!InventoryStackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not display item: No slots available!"), *FString(__FUNCTION__));
		return false;
	}

	// Get an available display slot.
	const int32 SlotIndex = FindAvailableSlot();
	if (SlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not display item: No slots available!"), *FString(__FUNCTION__));
		return false;
	}

	const bool bIsRequestSuccess = ASyncRequestDisplay(InventoryStackId, SlotIndex);

	return bIsRequestSuccess;
}

bool ATartarusDisplayCase::RemoveFromDisplay(const FInventoryStackId& InventoryStackId)
{
	if (!InventoryComponent->Contains(InventoryStackId))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item not in inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the slot that has its display removed.
	int32 SlotIndex = INDEX_NONE;
	for (int32 i = 0; i < DisplaySlots.Num(); i++)
	{
		const FDisplayCaseSlot& DisplaySlot = DisplaySlots[i];

		if (DisplaySlot.GetInventoryStackId() != InventoryStackId)
		{
			continue;
		}
		SlotIndex = i;
	}

	if (SlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item not being displayed!"), *FString(__FUNCTION__));
		return false;
	}

	ATartarusItemInstance* const DisplayInstance = DisplaySlots[SlotIndex].GetDisplayItem().Get();

	// Get the itemsubssytem to despawn the display item.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const bool bIsDespawned = ItemSubsystem->DespawnItem(DisplayInstance);
	if (!bIsDespawned)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to remove item from display: Item not despawned!"), *FString(__FUNCTION__));
		return false;
	}

	// Clear the DisplaySlots entry,
	DisplaySlots[SlotIndex].SetDisplayedItem(FInventoryStackId(), nullptr);

	// Remove the item from the inventory.
	InventoryComponent->RetrieveEntry(InventoryStackId, 1);

	return true;
}

int32 ATartarusDisplayCase::FindAvailableSlot() const
{
	for (int32 i = 0; i < DisplaySlots.Num(); i++)
	{
		if (!DisplaySlots[i].IsAvailable())
		{
			continue;
		}

		return i;
	}

	return INDEX_NONE;
}

void ATartarusDisplayCase::HandleDisplayRequestCompleted(ATartarusItemInstance* const DisplayItem, const int32 DisplaySlotIndex)
{
	DisplaySlots[DisplaySlotIndex].SetDisplayedItem(DisplaySlots[DisplaySlotIndex].GetInventoryStackId(), DisplayItem);

	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	UPrimitiveComponent* PhysicsComponent = Cast<UPrimitiveComponent>(DisplayItem->GetRootComponent());
	PhysicsComponent->SetSimulatePhysics(false);

	DisplayItem->AttachToActor(this, AttachmentRules);

	const FVector RelativeLocation = DisplaySlots[DisplaySlotIndex].GetSlotRelativeLocation();
	DisplayItem->SetActorRelativeLocation(RelativeLocation);
}

void ATartarusDisplayCase::HandleArtifactsDataReceived(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ArtifactsData)
{
	// Get the inventory of the player.
	AController* const PlayerController = GetWorld()->GetFirstPlayerController<AController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to Treasure: No player in the world!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const PlayerInventory = PlayerController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(PlayerInventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to Treasure: No inventory on player!"), *FString(__FUNCTION__));
		return;
	}

	// Loop over each artifact, and look if the player has it in their inventory.
	for (const UTartarusItem* const Artifact : ArtifactsData)
	{
		// Get all stacks that contains the current artifact.
		const TArray<const FInventoryStack*> InventoryArtifacts = PlayerInventory->GetOverviewMulti(EInventoryType::Artifact, Artifact->GetPrimaryAssetId());
	
		// For each stack, try to empty the whole stack and add it to the display.
		for (const FInventoryStack* const InventoryStack : InventoryArtifacts)
		{
			// Keep reducing the current stack StackCount untill none are left in the inventory.
			for (int32 i = 0; i < InventoryStack->StackSize; i++)
			{
				const FPrimaryAssetId ArtifactId = InventoryStack->GetEntryId();
	
				// Retrieve the artifact from the player invetory.
				const bool bHasRetrieved = PlayerInventory->RetrieveEntry(EInventoryType::Artifact, ArtifactId, 1);
				if (!bHasRetrieved)
				{
					continue;
				}
	
				const bool bAddedToDisplay = AddToDisplay(Artifact);
				if (bAddedToDisplay)
				{
					continue;
				}
	
				// If the item was not added to the display, give it back to the player.
				PlayerInventory->StoreEntry(Artifact, 1);
			}
		}
	}
}

#pragma region ASyncDisplay
bool ATartarusDisplayCase::ASyncRequestDisplay(const FInventoryStackId& InventoryStackId, const int32 SlotIndex)
{
	// Check that the item to display exists in the inventory.
	if (!InventoryComponent->Contains(InventoryStackId))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: stack not in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the item subsystem.
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: World is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Request data of the item to display.
	FGetItemDataRequestCompletedEvent OnDataRequestCompleted;
	OnDataRequestCompleted.AddUObject(this, &ATartarusDisplayCase::HandleItemDataLoaded);
	
	// Get the overview of the stack.
	const FInventoryStack* ItemStack = InventoryComponent->GetOverviewSingle(InventoryStackId);
	
	TArray<FPrimaryAssetId> ToSpawnItemIds;
	ToSpawnItemIds.Add(ItemStack->GetEntryId());
	
	FGuid ASyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ToSpawnItemIds, OnDataRequestCompleted);
	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not request the item data!"), *FString(__FUNCTION__));
		return false;
	}
	
	FDisplayRequestInfo DisplayRequest = FDisplayRequestInfo(SlotIndex);
	DisplayRequest.SetASyncLoadRequestId(ASyncRequestId);
	DisplayRequests.Add(DisplayRequest);
	
	// Occupy the slot untill the request finishes.
	DisplaySlots[SlotIndex].SetDisplayedItem(InventoryStackId, nullptr);

	return true;
}

void ATartarusDisplayCase::HandleRequestCompleted(const FDisplayRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemInstance> DisplayItem)
{
	if (!CompletedRequest)
	{
		return;
	}

	// If no item was displayed, free up the slot.
	if (!IsValid(DisplayItem.Get()))
	{
		DisplaySlots[CompletedRequest->GetDisplaySlot()].SetDisplayedItem(FInventoryStackId(), nullptr);
	}

	DisplayRequests.RemoveSingleSwap(*CompletedRequest);
}

void ATartarusDisplayCase::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FDisplayRequestInfo* const CurrentRequest = DisplayRequests.FindByPredicate([&ASyncLoadRequestId](const FDisplayRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	// Get the item subsystem.
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: World is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	FItemSpawnRequestCompletedEvent OnItemsSpawned;
	OnItemsSpawned.AddUObject(this, &ATartarusDisplayCase::HandleItemSpawned);
	
	FGuid ASyncRequestId = ItemSubsystem->AsyncRequestSpawnItems(ItemsData, FTransform::Identity, ItemSpawnParameters, OnItemsSpawned);
	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not request the item data!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
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

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	// Assign the Spawned actor to the slot the request ocupies.
	if (SpawnedItems.IsEmpty() || !SpawnedItems[0].IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to display item: No item spawned!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	const int32 SlotIndex = CurrentRequest->GetDisplaySlot();
	ATartarusItemInstance* const ItemToDisplay = SpawnedItems[0].Get();

	// [Koen Goossens] TODO: This should be executed trough a delegate on the request.
	HandleDisplayRequestCompleted(ItemToDisplay, SlotIndex);
	
	HandleRequestCompleted(CurrentRequest, SpawnedItems[0]);
}
#pragma endregion

#pragma region TartarusInteractableTargetInterface
bool ATartarusDisplayCase::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusDisplayCase::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// Get the inventory of the instigator.
	UTartarusInventoryComponent* const PlayerInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(PlayerInventory))
	{
		return false;
	}

	// Get the number of available slots to display.
	int32 NumAvailableSlots = 0;

	for (const FDisplayCaseSlot& DisplaySlot : DisplaySlots)
	{
		if (DisplaySlot.IsAvailable())
		{
			NumAvailableSlots++;
		}
	}

	if (NumAvailableSlots <= 0)
	{
		return false;
	}

	// Find all treasure keys in the inventory of the player, by first getting all treasure key id's from the ItemManager.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Spawn and link key to treasure: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	TArray<EItemType> ItemTypes;
	ItemTypes.Add(EItemType::Artifact);
	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusDisplayCase::HandleArtifactsDataReceived);

	const FGuid AsyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemTypes, OnRequestCompleted);
	if (!AsyncRequestId.IsValid())
	{
		return false;
	}

	return true;
}
#pragma endregion