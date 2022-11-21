// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusEquipableManager.generated.h"

class ATartarusItemBase;

struct FItemTableRow;

#pragma region EquipmentData
UENUM(BlueprintType, meta = (Bitflags))
enum class EEquipmentSlot : uint8
{
	None = 0x00,
	LeftHand = 0x01,
	RightHand = 0x02,
};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
	GENERATED_BODY()

public:
	/*
	* Retrieves the item that is being held by this slot.
	* Return: Pointer to the item being held, nullptr if no item exist.
	*/
	ATartarusItemBase* GetItem() const { return Item.Get(); }

	// Sets the Item instance that is equipped to this slot.
	void SetItem(ATartarusItemBase* const ItemInstance) { Item = ItemInstance; }

	/*
	* Retrieves the InventoryStackId that is used to reference the equipped item in the owner inventory.
	* Return: Id of the ItemStack that represents the equipped item in the owner inventory.
	*/
	const FGuid& GetInventoryStackId() const { return InventoryStackId; }

	void SetInventoryItemStackId(const FGuid& InventoryStackIdReference);

	/*
	* Retrieves the name of the socket that is linked to this slot.
	* Return: Name of the socket to attach to.
	*/
	const FName& GetSocket() { return Socket; }

	// Resets all data stored.
	void Reset();

protected:
	// Optional Socket name to attach to.
	UPROPERTY(EditDefaultsOnly)
		FName Socket = NAME_None;

private:
	TWeakObjectPtr<ATartarusItemBase> Item = nullptr;
	FGuid InventoryStackId = FGuid();
};
#pragma endregion

#pragma region ASyncEquip
USTRUCT()
struct FEquipRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FEquipRequestInfo() {}
	FEquipRequestInfo(const EEquipmentSlot Slot);

	EEquipmentSlot GetEquipSlot() const { return EquipSlot; }

private:
	EEquipmentSlot EquipSlot = EEquipmentSlot::None;
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
	bool Unequip(const FGuid& InventoryStackId);

	/*
	* Searches for an equipped item.
	* Return: the Equipped info of the item, nullptr if the item is not equipped.
	*/
	const FEquipmentInfo* FindEquippedItem(const ATartarusItemBase* const ToFindItem) const;

protected:
	// All possible slots with related data.
	UPROPERTY(EditDefaultsOnly)
		TMap<EEquipmentSlot, FEquipmentInfo> EquipmentSlots;

	/*
	* Finds a slot that is available to equip on.
	* Return: A slot that is available to equip on, EEquipmentSlot::None if no slot is available.
	*/
	EEquipmentSlot FindAvailableRequestedSlot(const EEquipmentSlot RequestedSlot) const;

	/*
	* Called when a change in the inventory happens, used to unequip items that are no longer in the inventory.
	*/
	void HandleInventoryUpdated(EInventoryChanged ChangeType, FGuid StackId, int32 StackSize);

#pragma region ASyncEquip
public:
	/*
	* Equips an actor to the owner.
	* Return: True, started request to equip the item.
	*/
	bool ASyncRequestEquip(const FGuid& InventoryStackId, const EEquipmentSlot SlotName);

protected:
	void HandleRequestCompleted(const FEquipRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemBase> EquippedItem);

	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData);
	void HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItems);

private:
	TArray<FEquipRequestInfo> EquipRequests;
#pragma endregion
};
