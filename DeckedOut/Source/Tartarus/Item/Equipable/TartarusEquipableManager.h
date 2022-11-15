// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusEquipableManager.generated.h"

class ATartarusItemBase;

struct FItemTableRow;

UENUM(BlueprintType, meta = (Bitflags))
enum class EEquipmentSlot : uint8
{
	None = 0x00,
	LeftHand = 0x01,
	RightHand = 0x02,
};

USTRUCT()
struct FEquipmentInfo
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<ATartarusItemBase> Item = nullptr;
	FGuid InventoryStackId = FGuid();
};

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

protected:
	// Map that identifies which Skeleton Socket corresponds to a slot.
	UPROPERTY(EditDefaultsOnly)
		TMap<EEquipmentSlot, FName> SocketMapping;

	TMap<EEquipmentSlot, FEquipmentInfo> EquipmentSlots;

	/*
	* Finds a slot that is available to equip on.
	* Return: A slot that is available to equip on, EEquipmentSlot::None if no slot is available.
	*/
	EEquipmentSlot FindAvailableRequestedSlot(const EEquipmentSlot RequestedSlot) const;

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
