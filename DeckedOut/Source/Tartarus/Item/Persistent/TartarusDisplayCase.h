// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusDisplayCase.generated.h"

class UTartarusInventoryComponent;
class ATartarusItemBase;

struct FItemTableRow;

USTRUCT(BlueprintType)
struct FDisplayCaseSlot
{
	GENERATED_BODY()

public:
	FDisplayCaseSlot() {}
	FDisplayCaseSlot(const FVector& SlotLocation);

	bool IsAvailable() const;
	void SetDisplayedItem(const FGuid& ItemInventoryStackId, ATartarusItemBase* const ToDisplay);
	
	const FGuid& GetInventoryStackId() const { return InventoryStackId; }
	const FVector& GetSlotRelativeLocation() const { return Location; }
	const TWeakObjectPtr<ATartarusItemBase>& GetDisplayItem() const { return Item; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FVector Location = FVector::ZeroVector;

private:
	TWeakObjectPtr<ATartarusItemBase> Item = nullptr;
	FGuid InventoryStackId = FGuid();
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
	bool AddToDisplay(const int32 ItemId);
	bool RemoveFromDisplay(const FGuid& InventoryStackId);

protected:
	UPROPERTY(EditDefaultsOnly)
		TArray<FDisplayCaseSlot> DisplaySlots;
	
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> InventoryComponent;

	int32 FindAvailableSlot() const;
	void HandleDisplayRequestCompleted(ATartarusItemBase* const DisplayItem, const int32 DisplaySlotIndex);

#pragma region ASyncDisplay
// [Koen Goossens] TODO: Refactor the TartarusEquipableManager as the workflow is the same.
protected:
	bool ASyncRequestDisplay(const FGuid& InventoryStackId, const int32 SlotIndex);

	void HandleRequestCompleted(const FDisplayRequestInfo* const CompletedRequest, const TWeakObjectPtr<ATartarusItemBase> DisplayItem);

	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<FItemTableRow> ItemsData);
	void HandleItemSpawned(FGuid ASyncLoadRequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedItems);

private:
	TArray<FDisplayRequestInfo> DisplayRequests;
#pragma endregion

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
