// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "Item/Inventory/TartarusInventoryData.h"
#include "Item/System/TartarusAsyncItemData.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusDisplayCase.generated.h"

class UTartarusInventoryComponent;
class UTartarusItem;
class ATartarusItemInstance;

struct FItemTableRow;

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

#pragma region ASyncDisplay
USTRUCT()
struct FDisplayRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FDisplayRequestInfo() {}
	FDisplayRequestInfo(const int32 Slot);

	int32 GetDisplaySlot() const { return SlotIndex; }

private:
	int32 SlotIndex = INDEX_NONE;
};
#pragma endregion

/*
* Actor that displays its inventory content.
*/
UCLASS()
class TARTARUS_API ATartarusDisplayCase : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusDisplayCase();

public:
	bool AddToDisplay(const UTartarusItem* const Item);
	bool RemoveFromDisplay(const FInventoryStackId& InventoryStackId);

protected:
	// Slots used to dispaly items in the inventory.
	UPROPERTY(EditDefaultsOnly)
		TArray<FDisplayCaseSlot> DisplaySlots;
	
	// Inventory component.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> InventoryComponent;

	// Spawn parameters for items to display.
	UPROPERTY(EditDefaultsOnly)
		FItemSpawnParameters ItemSpawnParameters = FItemSpawnParameters();

	int32 FindAvailableSlot() const;
	void HandleDisplayRequestCompleted(ATartarusItemInstance* const DisplayItem, const int32 DisplaySlotIndex);

	void HandleArtifactsDataReceived(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ArtifactsData);

#pragma region ASyncDisplay
// [Koen Goossens] TODO: Refactor the TartarusEquipableManager as the workflow is the same.
protected:
	bool ASyncRequestDisplay(const FInventoryStackId& InventoryStackId, const int32 SlotIndex);

	void HandleRequestCompleted(const FDisplayRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemInstance> DisplayItem);

	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedItems);

private:
	TArray<FDisplayRequestInfo> DisplayRequests;
#pragma endregion

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
	virtual void DisableInteraction() override;
#pragma endregion
};
