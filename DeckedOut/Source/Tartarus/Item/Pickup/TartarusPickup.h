// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "Item/TartarusItemInstance.h"
#include "System/TartarusAsyncLoadData.h"

#include "TartarusPickup.generated.h"

class UTartarusInventoryComponent;
class UTartarusItem;
class UTartarusWidgetComponent;

#pragma region ASyncLoading
USTRUCT()
struct FPickupRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FPickupRequestInfo() {}
	FPickupRequestInfo(UTartarusInventoryComponent* const InstigatorInventory, const bool bIsPickupRequest);

	UTartarusInventoryComponent* GetInventory() const { return Inventory.Get(); }
	bool ShouldPickup() { return bShouldPickup; }

private:
	TWeakObjectPtr<UTartarusInventoryComponent> Inventory = nullptr;
	bool bShouldPickup = false;
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
	virtual void Initialize(const FPrimaryAssetId ItemReferenceId) override;

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	/*
	* Defines the behavior to execute when this item is pickedup in the world.
	* Return: Has the behavior been handles succesfully or not.
	*/
	virtual bool PickupItemAsync(const TObjectPtr<AController> InstigatorController);
	bool PickupItem(UTartarusInventoryComponent* const Inventory, const UTartarusItem* const Item);

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
	virtual void DisableInteraction() override;
	virtual bool ToggleInteractionPrompt(const bool bShowPrompt);

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusWidgetComponent> InteractionWidgetComponent = nullptr;

	void CreateInteractionWidgetComponent();
	bool SetupInteractionPrompt(const UTartarusItem* const Item);

private:
	bool bIsInteractable = true;
#pragma endregion

#pragma region AsyncLoading
protected:
	FGuid RequestItemData();
	void HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData);
	void HandleRequestCompleted(const FPickupRequestInfo* const CompletedRequest);

private:
	TArray<FPickupRequestInfo> PickupRequests;
#pragma endregion
};
