// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableManager.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Item/Equipable/TartarusEquipableInterface.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "Engine/World.h"

#pragma region ASyncEquip
FEquipRequestInfo::FEquipRequestInfo(const FInventoryStackId& StackId, const EEquipmentSlot Slots)
{
	RequestId = FGuid::NewGuid();

	InventoryStackId = StackId;
	RequestedSlots = Slots;
}
#pragma endregion

// Sets default values for this component's properties
UTartarusEquipableManager::UTartarusEquipableManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTartarusEquipableManager::BeginPlay()
{
	Super::BeginPlay();

	// [Koen Goossens] TODO: Check this trough IsDataValid to throw a BP error instead of cheking this only at runtime.
	// Check if the slots are setup.
	if (EquipmentSlots.IsEmpty())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Equipment mapping is not setup!"), *FString(__FUNCTION__));
		return;
	}

	// Subscribe to inventory updates to unequip if an item is removed.
	UTartarusInventoryComponent* const Inventory = GetOwnerInventory();
	if (IsValid(Inventory))
	{
		Inventory->OnInventoryEntryUpdated().AddUObject(this, &UTartarusEquipableManager::HandleInventoryUpdated);
	}
}

bool UTartarusEquipableManager::Unequip(const FInventoryStackId& InventoryStackId)
{
	// Find the equipped item using the inventory stack Id.
	for (TPair<EEquipmentSlot, FEquipmentInfo>& EquipmentSlot : EquipmentSlots)
	{
		if (EquipmentSlot.Value.GetInventoryStackId() != InventoryStackId)
		{
			continue;
		}

		// Clear the entry in the slots, and keep a reference to the item instance for further actions.
		ATartarusItemInstance* const ToUnequip = EquipmentSlot.Value.GetItem();
		EquipmentSlot.Value.Reset();

		if (!IsValid(ToUnequip))
		{
			return true;
		}

		if (ItemSpawnParameters.ShouldItemSubystemManage())
		{
			UE_LOG(LogTartarus, Error, TEXT("%s: Failed to unequip item, Expecting the equipable manager to manage the item but its not set to!"), *FString(__FUNCTION__));
			return false;
		}

		ToUnequip->Destroy();

		return true;
	}

	return false;
}

const FEquipmentInfo* UTartarusEquipableManager::FindEquippedItem(const ATartarusItemInstance* const ToFindItem) const
{
	if (!IsValid(ToFindItem))
	{
		return nullptr;
	}

	for (const TPair<EEquipmentSlot, FEquipmentInfo>& EquipmentSlot : EquipmentSlots)
	{
		const ATartarusItemInstance* const EquippedItem = EquipmentSlot.Value.GetItem();

		if (!IsValid(EquippedItem))
		{
			continue;
		}

		if (EquippedItem != ToFindItem)
		{
			continue;
		}

		return &EquipmentSlot.Value;
	}

	return nullptr;
}

void UTartarusEquipableManager::SetEquipableMeshComponent(USkeletalMeshComponent* const Mesh)
{
	if (!IsValid(Mesh))
	{
		return;
	}

	EquipableMeshComponent = Mesh;

	for (auto Slot : EquipmentSlots)
	{
		if (!IsValid(Slot.Value.GetItem()))
		{
			continue;
		}

		AttachToMesh(Slot.Key);
	}
}

EEquipmentSlot UTartarusEquipableManager::FindAvailableSlot(const uint8 SlotsMask) const
{
	for (TPair<EEquipmentSlot, FEquipmentInfo> EquipmentSlot : EquipmentSlots)
	{
		const uint8 Current = uint8(EquipmentSlot.Key);
		const uint8 BitResult = SlotsMask & Current;

		if (BitResult != Current)
		{
			continue;
		}

		const bool bIsSlotAvailable = !EquipmentSlot.Value.GetInventoryStackId().IsValid();
		if (!bIsSlotAvailable)
		{
			continue;
		}

		return EquipmentSlot.Key;
	}

	return EEquipmentSlot::None;
}

bool UTartarusEquipableManager::AttachToMesh(const EEquipmentSlot Slot)
{
	// Attach the spawned item to the owner and tell the item it is equipped.
	if (!IsValid(EquipableMeshComponent))
	{
		return false;
	}

	const FName& SocketName = EquipmentSlots[Slot].GetSocket();
	if (SocketName == NAME_None)
	{
		return false;
	}

	ATartarusItemInstance* const ItemRaw = EquipmentSlots[Slot].GetItem();
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	ItemRaw->AttachToComponent(EquipableMeshComponent, AttachmentRules, SocketName);

	return true;
}

#pragma region ASyncEquip
bool UTartarusEquipableManager::ASyncRequestEquip(const FInventoryStackId& InventoryStackId, const EEquipmentSlot RequestedSlots)
{
	// Make a request to get the Item specifc data.
	const FGuid ASyncRequestId = RequestItemData(InventoryStackId);
	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not request the item data!"), *FString(__FUNCTION__));
		return false;
	}

	FEquipRequestInfo EquipRequest = FEquipRequestInfo(InventoryStackId, RequestedSlots);
	EquipRequest.SetASyncLoadRequestId(ASyncRequestId);
	EquipRequests.Add(EquipRequest);

	return true;
}

void UTartarusEquipableManager::HandleRequestCompleted(const FEquipRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemInstance> EquippedItem)
{
	if (!CompletedRequest)
	{
		return;
	}

	// If no item was equipped, free up the slot.
	if (!IsValid(EquippedItem.Get()) && EquipmentSlots.Contains(CompletedRequest->ReservedSlot))
	{
		EquipmentSlots[CompletedRequest->ReservedSlot].Reset();
	}

	EquipRequests.RemoveSingleSwap(*CompletedRequest);
}

void UTartarusEquipableManager::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FEquipRequestInfo* const CurrentRequest = EquipRequests.FindByPredicate([&ASyncLoadRequestId](const FEquipRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	if (ItemsData.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: No Item data is loaded!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	// Check if the item can be equipped in the reserved slot and reserve that slot.
	const bool bCouldReserveSlot = ReserveSlot(CurrentRequest, ItemsData[0]);
	if (!bCouldReserveSlot)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to equip: Could not reserve a slot"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}
	
	// Create a request to spawn the item and update the request.
	const FGuid ASyncRequestId = RequestItemsSpawn(ItemsData);
	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not request the item data!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}
	
	CurrentRequest->SetASyncLoadRequestId(ASyncRequestId);
}

void UTartarusEquipableManager::HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedItems)
{
	// Get the request that is being handled.
	FEquipRequestInfo* const CurrentRequest = EquipRequests.FindByPredicate([&ASyncLoadRequestId](const FEquipRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	// Validate that an equipable item to equip has been spawned.
	if (SpawnedItems.IsEmpty() || !SpawnedItems[0].IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: No item spawned!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	ITartarusEquipableInterface* const EquipableInterface = Cast<ITartarusEquipableInterface>(SpawnedItems[0]);
	if (!EquipableInterface)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Spawned item is not an equipable item!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	// Assign the spawned item to the reserved slot.
	EEquipmentSlot ReservedSlot = CurrentRequest->ReservedSlot;
	EquipmentSlots[ReservedSlot].SetItem(SpawnedItems[0].Get());

	if (AttachToMesh(ReservedSlot))
	{
		EquipableInterface->OnEquipped(GetOwner());
	}

	return HandleRequestCompleted(CurrentRequest, SpawnedItems[0]);
}

FGuid UTartarusEquipableManager::RequestItemsSpawn(const TArray<UTartarusItem*>& ItemTableRows)
{
	// Get the item subsystem.
	const UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to request spawn of items: World is invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to request spawn of items: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FItemSpawnRequestCompletedEvent OnItemsSpawned;
	OnItemsSpawned.AddUObject(this, &UTartarusEquipableManager::HandleItemSpawned);

	const FGuid ASyncRequestId = ItemSubsystem->AsyncRequestSpawnItems(ItemTableRows, FTransform::Identity, ItemSpawnParameters, OnItemsSpawned);

	return ASyncRequestId;
}

FGuid UTartarusEquipableManager::RequestItemData(const FInventoryStackId& InventoryStackId)
{
	// Check if all components are accesible.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to request Item Data: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	UTartarusInventoryComponent* const Inventory = GetOwnerInventory();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: inventory is invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Confirm that the inventory stack is from this owner.
	if (!Inventory->Contains(InventoryStackId))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Item stack not in the inventory!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Prepare a callback for when the data is loaded.
	FGetItemDataRequestCompletedEvent OnDataRequestCompleted;
	OnDataRequestCompleted.AddUObject(this, &UTartarusEquipableManager::HandleItemDataLoaded);

	// Create a new request to the ItemSubsytem to load item data.
	const FInventoryStack* const ItemStack = Inventory->GetOverviewSingle(InventoryStackId);
	TArray<FPrimaryAssetId> ToSpawnItemIds;
	ToSpawnItemIds.Add(ItemStack->GetEntryId());
	
	const FGuid ASyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ToSpawnItemIds, OnDataRequestCompleted);

	return ASyncRequestId;
}

bool UTartarusEquipableManager::ReserveSlot(FEquipRequestInfo* const ASyncRequest, const UTartarusItem* const ItemData)
{
	uint8 RequestedSlotsMask = (uint8)ASyncRequest->GetRequestedSlots();
	
	// If the request had no requestedslots, use any of the ItemData slots otherwise only use requested valid slots.
	RequestedSlotsMask = RequestedSlotsMask == 0 ? ItemData->EquipableSlots : RequestedSlotsMask & ItemData->EquipableSlots;

	if (RequestedSlotsMask == 0)
	{
		return false;
	}

	const EEquipmentSlot AvailableSlot = FindAvailableSlot(RequestedSlotsMask);
	if (AvailableSlot == EEquipmentSlot::None)
	{
		return false;
	}

	ASyncRequest->ReservedSlot = AvailableSlot;
	EquipmentSlots[AvailableSlot].SetInventoryItemStackId(ASyncRequest->GetInventoryStackId());

	return true;
}
#pragma endregion

#pragma region InventoryUpdates
UTartarusInventoryComponent* UTartarusEquipableManager::GetOwnerInventory()
{
	// If the current saved inventory is valid, return it.
	if (OwnerInventory.IsValid())
	{
		return OwnerInventory.Get();
	}

	// No valid reference to the owner inventory is cached, search for it.
	const APawn* const OwningPawn = GetOwner<APawn>();
	const AController* const Controller = IsValid(OwningPawn) ? OwningPawn->GetController() : GetOwner<AController>();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the owner inventory: Unable to retrieve controller!"), *FString(__FUNCTION__));
		return nullptr;
	}

	UTartarusInventoryComponent* const Inventory = Controller->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find the owner inventory: Could not find inventory!"), *FString(__FUNCTION__));
		return nullptr;
	}

	OwnerInventory = Inventory;

	return Inventory;
}

void UTartarusEquipableManager::HandleInventoryUpdated(EInventoryChanged ChangeType, FInventoryStackId InventoryStackId, int32 StackSize)
{
	// Something in the inventory changed,
	switch (ChangeType)
	{
	case EInventoryChanged::Stored:
	{
		HandleInventoryItemStored(InventoryStackId);
		break;
	}
	case EInventoryChanged::Retrieved:
	{
		HandleInventoryItemRetrieved(InventoryStackId, StackSize);
		break;
	}
	default:
		break;
	}
}

void UTartarusEquipableManager::HandleInventoryItemRetrieved(const FInventoryStackId& InventoryStackId, const int32 StackSize)
{
	for (TPair<EEquipmentSlot, FEquipmentInfo> Slot : EquipmentSlots)
	{
		// Continue loop if the change is not related to this slot.
		if (Slot.Value.GetInventoryStackId() != InventoryStackId)
		{
			continue;
		}

		// Continue loop if the change resulted in a higher than 0 stack size.
		if (StackSize > 0)
		{
			continue;
		}

		// The change resulted in the item no longer being in the inventory, unequip and destroy it.
		if (IsValid(Slot.Value.GetItem()))
		{
			Unequip(Slot.Value.GetInventoryStackId());
		}

		Slot.Value.Reset();
	}
}

void UTartarusEquipableManager::HandleInventoryItemStored(const FInventoryStackId& InventoryStackId)
{
	// The inventory received a new item, if this is to be auto-equipped do so.
	UTartarusInventoryComponent* const Inventory = GetOwnerInventory();
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not find inventory!"), *FString(__FUNCTION__));
		return;
	}

	const FInventoryStack* const StoredItemStack = Inventory->GetOverviewSingle(InventoryStackId);
	if (!StoredItemStack)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not find ItemStack in inventory!"), *FString(__FUNCTION__));
		return;
	}

	TArray<FPrimaryAssetId> ItemIds;
	ItemIds.Add(StoredItemStack->GetEntryId());

	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusEquipableManager::HandleInventoryItemDataLoaded);

	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not find ItemSubsystem!"), *FString(__FUNCTION__));
		return;
	}

	const FGuid ASyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ItemIds, OnRequestCompleted);
	if (!ASyncRequestId.IsValid())
	{
		return;
	}

	FEquipRequestInfo EquipRequest = FEquipRequestInfo(InventoryStackId, EEquipmentSlot::None);
	EquipRequest.SetASyncLoadRequestId(ASyncRequestId);
	EquipRequests.Add(EquipRequest);
}

void UTartarusEquipableManager::HandleInventoryItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// For each item that got loaded, check if it can be equipped.
	for (const UTartarusItem* const ItemData : ItemsData)
	{
		if (!ItemData->bCanAutoEquip)
		{
			continue;
		}
		
		HandleItemDataLoaded(ASyncLoadRequestId, ItemsData);
	}
}
#pragma endregion
