// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Objects/TartarusInteractablePopupActor.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "Item/System/TartarusAsyncItemData.h"

#include "TartarusDisplayCase.generated.h"

class UTartarusInventoryComponent;
class ATartarusItemInstance;

USTRUCT(BlueprintType)
struct FDisplayCaseSlot
{
	GENERATED_BODY()

public:
	FDisplayCaseSlot() {}
	FDisplayCaseSlot(const FVector& SlotLocation);

	bool IsAvailable() const;
	void SetDisplayedItem(const FInventoryStackId& ItemInventoryStackId, ATartarusItemInstance* const ToDisplay);
	
	const FInventoryStackId& GetInventoryStackId() const { return InventoryStackId; }
	const FVector& GetSlotRelativeLocation() const { return Location; }
	const TWeakObjectPtr<ATartarusItemInstance>& GetDisplayItem() const { return Item; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FVector Location = FVector::ZeroVector;

private:
	TWeakObjectPtr<ATartarusItemInstance> Item = nullptr;
	FInventoryStackId InventoryStackId = FInventoryStackId();
};

/*
* Actor that displays its inventory content.
*/
UCLASS()
class TARTARUS_API ATartarusDisplayCase : public ATartarusInteractablePopupActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusDisplayCase();

#pragma region Display
protected:
	// Slots used to display items in the inventory.
	UPROPERTY(EditDefaultsOnly)
		TArray<FDisplayCaseSlot> DisplaySlots;

	// Spawn parameters for items to display.
	UPROPERTY(EditDefaultsOnly)
		FItemSpawnParameters ItemSpawnParameters = FItemSpawnParameters();

	void UpdateDisplaySlots();
	void DisplayItemASync(FDisplayCaseSlot& Slot, const FInventoryStack& InventoryStack);
	void DisplayItem(FDisplayCaseSlot& Slot, const FInventoryStack& InventoryStack, ATartarusItemInstance* const ItemInstance);
	void RemoveItem(FDisplayCaseSlot& Slot);
#pragma endregion

#pragma region Inventory
protected:
	// Inventory component.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> InventoryComponent;

	void OnInventoryEntryChanged(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize);
#pragma endregion

#pragma region AsyncLoad
private:
	struct FDisplayRequestInfo : public FASyncLoadRequest
	{
	public:
		FDisplayRequestInfo() {}
		FDisplayRequestInfo(FDisplayCaseSlot* Slot, const FInventoryStack* ToDisplayStack);

		FDisplayCaseSlot* const GetDisplaySlot() { return DisplaySlot; };
		const FInventoryStack* const GetInventoryStack() { return InventoryStack; };

	private:
		FDisplayCaseSlot* DisplaySlot = nullptr;
		const FInventoryStack* InventoryStack = nullptr;
	};

protected:
	FGuid RequestItemDataAsync(TArray<FPrimaryAssetId> AssetsToLoad);
	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedItems);
	void HandleRequestCompleted(const FDisplayRequestInfo* const CompletedRequest);

private:
	TArray<FDisplayRequestInfo> DisplayRequests;
#pragma endregion
};
