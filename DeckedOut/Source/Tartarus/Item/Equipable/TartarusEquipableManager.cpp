// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/TartarusEquipableManager.h"

#include "GameFramework/Character.h"
#include "Item/Equipable/TartarusEquipableInterface.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItemBase.h"
#include "Logging/TartarusLogChannels.h"

#pragma region EquipmentData
void FEquipmentInfo::SetInventoryItemStackId(const FGuid& InventoryStackIdReference)
{
	if (InventoryStackId.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Overwritting a valid InventoryStackId!"), *FString(__FUNCTION__));
	}

	InventoryStackId = InventoryStackIdReference;
}

void FEquipmentInfo::Reset()
{
	Item = nullptr;
	InventoryStackId = FGuid();
}
#pragma endregion

#pragma region ASyncEquip
FEquipRequestInfo::FEquipRequestInfo(const EEquipmentSlot Slot)
{
	RequestId = FGuid::NewGuid();

	EquipSlot = Slot;
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

	// Check if the slots are setup.
	if (EquipmentSlots.IsEmpty())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Equipment mapping is not setup!"), *FString(__FUNCTION__));
		return;
	}

	// Subscribe to inventory updates to unequip if an item is removed.
	const APawn* const OwningPawn = GetOwner<APawn>();

	const AController* const Controller = IsValid(OwningPawn) ? OwningPawn->GetController() : GetOwner<AController>();
	
	if (!IsValid(Controller))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to retrieve controller!"), *FString(__FUNCTION__));
		return;
	}

	UTartarusInventoryComponent* const Inventory = Controller->FindComponentByClass<UTartarusInventoryComponent>();

	if(!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not find inventory!"), *FString(__FUNCTION__));
		return;
	}

	Inventory->OnInventoryChanged().AddUObject(this, &UTartarusEquipableManager::HandleInventoryUpdated);
}

bool UTartarusEquipableManager::Unequip(const FGuid& InventoryStackId)
{
	// Find the equipped item using the inventory stack Id.
	for (TPair<EEquipmentSlot, FEquipmentInfo>& EquipmentSlot : EquipmentSlots)
	{
		if (EquipmentSlot.Value.GetInventoryStackId() != InventoryStackId)
		{
			continue;
		}

		// Clear the entry in the slots, and keep a reference to the item instance for further actions.
		ATartarusItemBase* const ToUnequip = EquipmentSlot.Value.GetItem();
		EquipmentSlot.Value.Reset();

		if (!IsValid(ToUnequip))
		{
			return true;
		}

		// The item still exist as an instance, detach it.
		const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);	
		ToUnequip->DetachFromActor(DetachmentRules);

		// [Koen Goossens] TODO: Magic Number 100.
		ToUnequip->SetActorLocation(ToUnequip->GetActorLocation() + ToUnequip->GetActorForwardVector() * 100);

		ITartarusEquipableInterface* const EquipableInterface = Cast<ITartarusEquipableInterface>(ToUnequip);
		if (EquipableInterface)
		{
			EquipableInterface->OnUnequipped();
		}

		return true;
	}

	return false;
}

const FEquipmentInfo* UTartarusEquipableManager::FindEquippedItem(const ATartarusItemBase* const ToFindItem) const
{
	if (!IsValid(ToFindItem))
	{
		return nullptr;
	}

	for (const TPair<EEquipmentSlot, FEquipmentInfo>& EquipmentSlot : EquipmentSlots)
	{
		const ATartarusItemBase* const EquippedItem = EquipmentSlot.Value.GetItem();

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

#pragma optimize("", off)
EEquipmentSlot UTartarusEquipableManager::FindAvailableRequestedSlot(const EEquipmentSlot RequestedSlot) const
{
	for (TPair<EEquipmentSlot, FEquipmentInfo> EquipmentSlot : EquipmentSlots)
	{
		const uint8 Request = uint8(RequestedSlot);
		const uint8 Current = uint8(EquipmentSlot.Key);
		const uint8 BitResult = Request & Current;

		if (BitResult == Current)
		{
			const EEquipmentSlot FoundSlot = static_cast<EEquipmentSlot>(BitResult);

			if (!EquipmentSlots[FoundSlot].GetInventoryStackId().IsValid())
			{
				return FoundSlot;
			}
		}
	}

	return EEquipmentSlot::None;
}
#pragma optimize("", on)

void UTartarusEquipableManager::HandleInventoryUpdated(EInventoryChanged ChangeType, FGuid StackId, int32 StackSize)
{
	// Something in the inventory changed, check if it was a removal of an equipped item.
	if (ChangeType != EInventoryChanged::Retrieved)
	{
		return;
	}

	for (TPair<EEquipmentSlot, FEquipmentInfo> Slot : EquipmentSlots)
	{
		// Continue if the change is not related to this slot.
		if (Slot.Value.GetInventoryStackId() != StackId)
		{
			continue;
		}

		// Continue if the change resulted in a higher than 0 stack size.
		if (StackSize > 0)
		{
			continue;
		}

		// The change resulted in the item no longer being in the inventory, unequip and destroy it.
		if (IsValid(Slot.Value.GetItem()))
		{
			Unequip(Slot.Value.GetInventoryStackId());

			UWorld* const World = GetWorld();

			if (IsValid(World))
			{
				UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();

				if (IsValid(ItemSubsystem))
				{
					ItemSubsystem->DespawnItem(Cast<ATartarusItemBase>(Slot.Value.GetItem()));
				}
			}
		}

		Slot.Value = FEquipmentInfo();
	}
}

#pragma region ASyncEquip
bool UTartarusEquipableManager::ASyncRequestEquip(const FGuid& InventoryStackId, const EEquipmentSlot SlotName)
{
	// Is the requested slot available/does it exist?
	const EEquipmentSlot AvailableSlot = FindAvailableRequestedSlot(SlotName);
	if (AvailableSlot == EEquipmentSlot::None)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: All slots are used!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the owner inventory.
	APawn* const OwningPawn = Cast<APawn>(GetOwner());
	UTartarusInventoryComponent* const Inventory = OwningPawn->GetController()->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: inventory is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Check that the item to equip exists in the inventory.
	if (!Inventory->Contains(InventoryStackId))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Item stack not in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Get the item subsystem.
	UWorld* const World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: World is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();

	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Request data of the item to equip.
	FGetItemDataRequestCompletedEvent OnDataRequestCompleted;
	OnDataRequestCompleted.AddUObject(this, &UTartarusEquipableManager::HandleItemDataLoaded);

	// Get the overview of the stack.
	const FInventoryItemStack* const ItemStack = Inventory->GetOverviewSingle(InventoryStackId);

	TArray<int32> ToSpawnItemIds;
	ToSpawnItemIds.Add(ItemStack->GetItemId());

	FGuid ASyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ToSpawnItemIds, OnDataRequestCompleted);

	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not request the item data!"), *FString(__FUNCTION__));
		return false;
	}

	FEquipRequestInfo EquipRequest = FEquipRequestInfo(AvailableSlot);
	EquipRequest.SetASyncLoadRequestId(ASyncRequestId);
	EquipRequests.Add(EquipRequest);

	// Occupy the slot untill the request finishes.
	EquipmentSlots[AvailableSlot].SetInventoryItemStackId(InventoryStackId);

	return true;
}

void UTartarusEquipableManager::HandleRequestCompleted(const FEquipRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemBase> EquippedItem)
{
	if (!CompletedRequest)
	{
		return;
	}

	// If no item was equipped, free up the slot.
	if (!IsValid(EquippedItem.Get()))
	{
		EquipmentSlots[CompletedRequest->GetEquipSlot()] = FEquipmentInfo();
	}

	EquipRequests.RemoveSingleSwap(*CompletedRequest);
}

void UTartarusEquipableManager::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData)
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

	// Get the item subsystem.
	UWorld* const World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: World is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	UTartarusItemSubsystem* const ItemSubsystem = World->GetSubsystem<UTartarusItemSubsystem>();

	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	FItemSpawnRequestCompletedEvent OnItemsSpawned;
	OnItemsSpawned.AddUObject(this, &UTartarusEquipableManager::HandleItemSpawned);

	FGuid ASyncRequestId = ItemSubsystem->AsyncRequestSpawnItems(ItemsData, FTransform::Identity, OnItemsSpawned);

	if (!ASyncRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to equip: Could not request the item data!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	CurrentRequest->SetASyncLoadRequestId(ASyncRequestId);
}

void UTartarusEquipableManager::HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItems)
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

	// Assign the Spawned actor to the slot the request ocupies.
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

	EEquipmentSlot EquipSlot = CurrentRequest->GetEquipSlot();
	EquipmentSlots[EquipSlot].SetItem(SpawnedItems[0].Get());

	const ACharacter* const Owner = Cast<ACharacter>(GetOwner());

	if (Owner)
	{
		USkeletalMeshComponent* const AttachComponent = Owner->GetMesh();

		if (AttachComponent)
		{
			const FName& SocketName = EquipmentSlots[EquipSlot].GetSocket();

			if (SocketName != NAME_None)
			{
				ATartarusItemBase* const ItemRaw = EquipmentSlots[EquipSlot].GetItem();
				const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
				ItemRaw->AttachToComponent(AttachComponent, AttachmentRules, SocketName);

				EquipableInterface->OnEquipped(GetOwner());
			}
		}
	}

	return HandleRequestCompleted(CurrentRequest, SpawnedItems[0]);
}
#pragma endregion
