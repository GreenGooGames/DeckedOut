// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TartarusItemInstance.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "System/TartarusAsyncLoadData.h"

#include "TartarusPickup.generated.h"

class UTartarusInventoryComponent;
class UTartarusItem;

#pragma region ASyncLoading
USTRUCT()
struct FPickupRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FPickupRequestInfo() {}
	FPickupRequestInfo(UTartarusInventoryComponent* const InstigatorInventory);

	UTartarusInventoryComponent* GetInventory() const { return Inventory.Get(); }

private:
	TWeakObjectPtr<UTartarusInventoryComponent> Inventory = nullptr;
};
#pragma endregion

/**
 * A class for items that can be picked up and stored in the inventory without additional behavior.
 * To add extra behavior override HandlePickedup.
 */
UCLASS()
class TARTARUS_API ATartarusPickup : public ATartarusItemInstance, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:
	ATartarusPickup();

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	/*
	* Defines the behavior to execute when this item is pickedup in the world.
	* Return: Has the behavior been handles succesfully or not.
	*/
	virtual bool HandlePickedup(const TObjectPtr<AController> InstigatorController);

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion

#pragma region AsyncLoading
protected:
	FGuid RequestItemData();
	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleRequestCompleted(const FPickupRequestInfo* const CompletedRequest, const UTartarusItem* const ItemData);

private:
	TArray<FPickupRequestInfo> PickupRequests;
#pragma endregion
};
