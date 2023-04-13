// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Equipable/TartarusEquipableData.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusEquipableManager.generated.h"

class ATartarusItemBase;
class UTartarusInventoryComponent;

struct FItemTableRow;

#pragma region ASyncEquip
USTRUCT()
struct FEquipRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FEquipRequestInfo() {}
	FEquipRequestInfo(const FInventoryStackId& StackIds, const EEquipmentSlot Slots);

	EEquipmentSlot GetRequestedSlots() const { return RequestedSlots; }
	const FInventoryStackId& GetInventoryStackId() const { return InventoryStackId; }

	EEquipmentSlot ReservedSlot = EEquipmentSlot::None;

private:
	EEquipmentSlot RequestedSlots = EEquipmentSlot::None;
	FInventoryStackId InventoryStackId = FInventoryStackId();
};
#pragma endregion

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARTARUS_API UTartarusEquipableManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTartarusEquipableManager();

protected:
	virtual void BeginPlay() override;

public:
	/*
	* Unequips the actor if it is equipped.
	* Return: True, the actor was unequipped. False, Failed to unequip the actor.
	*/
	bool Unequip(const FInventoryStackId& InventoryStackId);

	/*
	* Searches for an equipped item.
	* Return: the Equipped info of the item, nullptr if the item is not equipped.
	*/
	const FEquipmentInfo* FindEquippedItem(const ATartarusItemBase* const ToFindItem) const;

	/*
	* Set the active mesh to equip to.
	* If any is equipped, they will be transfered.
	*/
	void SetEquipableMeshComponent(USkeletalMeshComponent* const Mesh);

protected:
	// How far away should the equipped object be moved to from the player when unequipping it.
	UPROPERTY(EditDefaultsOnly)
		float UnequipDistance = 250.0f;

	// All possible slots with related data.
	UPROPERTY(EditDefaultsOnly)
		TMap<EEquipmentSlot, FEquipmentInfo> EquipmentSlots;

	// The mesh to equip on.
	TObjectPtr<USkeletalMeshComponent> EquipableMeshComponent = nullptr;

	/*
	* Finds a slot that is available to equip on.
	* Return: A slot that is available to equip on, EEquipmentSlot::None if no slot is available.
	*/
	EEquipmentSlot FindAvailableSlot(const uint8 SlotsMask = 0) const;

	/*
	* Attaches the object in the slot to the Equippable mesh.
	* Return: True if the mesh is attached, false if attachment failed.
	*/
	bool AttachToMesh(const EEquipmentSlot Slot);

#pragma region ASyncEquip
public:
	/*
	* Equips an actor to the owner on the prefered slot.
	* Return: True, started request to equip the item.
	*/
	bool ASyncRequestEquip(const FInventoryStackId& InventoryStackId, const EEquipmentSlot RequestedSlots);

protected:
	void HandleRequestCompleted(const FEquipRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemBase> EquippedItem);

	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData);
	void HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItems);

	FGuid RequestItemsSpawn(const TArray<FItemTableRow>& ItemTableRows);
	FGuid RequestItemData(const FInventoryStackId& InventoryStackId);

	/*
	* Reserves a slot with priority to the RequestedSlots, if no slot is requested an avaialble slot that cen be used is selected.
	* Return: True if a slot is reserved for the request.
	*/
	bool ReserveSlot(FEquipRequestInfo* const ASyncRequest, const FItemTableRow& ItemData);

private:
	TArray<FEquipRequestInfo> EquipRequests;
#pragma endregion

#pragma region InventoryUpdates
protected:
	UTartarusInventoryComponent* GetOwnerInventory();

	/*
	* Called when a change in the inventory happens, used to unequip items that are no longer in the inventory.
	*/
	void HandleInventoryUpdated(EInventoryChanged ChangeType, FInventoryStackId InventoryStackId, int32 StackSize);

	void HandleInventoryItemRetrieved(const FInventoryStackId& InventoryStackId, const int32 StackSize);

	void HandleInventoryItemStored(const FInventoryStackId& InventoryStackId);
	void HandleInventoryItemDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData);

private:
	TWeakObjectPtr<UTartarusInventoryComponent> OwnerInventory = nullptr;
#pragma endregion
};
