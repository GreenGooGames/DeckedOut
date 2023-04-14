// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/TartarusPickup.h"

#include "Item/TartarusItem.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Logging/TartarusLogChannels.h"

#pragma region FPickupRequestInfo
FPickupRequestInfo::FPickupRequestInfo(UTartarusInventoryComponent* const InstigatorInventory)
{
	RequestId = FGuid::NewGuid();
	
	Inventory = InstigatorInventory;
}
#pragma endregion

ATartarusPickup::ATartarusPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	SetRootComponent(Mesh);
}

bool ATartarusPickup::HandlePickedup(const TObjectPtr<AController> InstigatorController)
{
	if (!PickupRequests.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Interaction Failed: Someone is already trying to pickup this!"), *FString(__FUNCTION__));
		return false;
	}

	// As base behavior, any item interacted with is stored into the instigator inventory.
	UTartarusInventoryComponent* const InstigatorInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(InstigatorInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Controller has no inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// create a request to retrieve the data of this item.
	const FGuid RequestId = RequestItemData();
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Failed to create a request to retrieve the item data!"), *FString(__FUNCTION__));
		return false;
	}

	FPickupRequestInfo PickupRequest = FPickupRequestInfo(InstigatorInventory);
	PickupRequest.SetASyncLoadRequestId(RequestId);
	PickupRequests.Add(PickupRequest);

	return true;
}

#pragma region TartarusInteractableTargetInterface
bool ATartarusPickup::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusPickup::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	if (!IsInteractable())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Item is not interactable!"), *FString(__FUNCTION__));
		return false;
	}

	if (!IsValid(InstigatorController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Controller is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	const bool bSuccess = HandlePickedup(InstigatorController);

	return bSuccess;
}
#pragma endregion

#pragma region AsyncLoading
FGuid ATartarusPickup::RequestItemData()
{
	// Check if all components are accesible.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to request Item Data: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return FGuid();
	}

	// Prepare a callback for when the data is loaded.
	FGetItemDataRequestCompletedEvent OnDataRequestCompleted;
	OnDataRequestCompleted.AddUObject(this, &ATartarusPickup::HandleItemDataLoaded);

	// Create a new request to the ItemSubsytem to load item data.
	TArray<FPrimaryAssetId> ToSpawnItemIds;
	ToSpawnItemIds.Add(GetItemId());

	const FGuid ASyncRequestId = ItemSubsystem->AsyncRequestGetItemsData(ToSpawnItemIds, OnDataRequestCompleted);

	return ASyncRequestId;
}

#pragma optimize("", off)
void ATartarusPickup::HandleItemDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> ItemsData)
{
	// Get the request that is being handled.
	FPickupRequestInfo* const CurrentRequest = PickupRequests.FindByPredicate([&ASyncLoadRequestId](const FPickupRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Pickup: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	if (ItemsData.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Pickup: No Item data is loaded!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	UTartarusInventoryComponent* const InstigatorInventory = CurrentRequest->GetInventory();
	if (!IsValid(InstigatorInventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Inventory no longer exists after interaction!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	const bool bIsStored = InstigatorInventory->StoreEntry(ItemsData[0], StackSize).IsValid();
	if (!bIsStored)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Could not store the item in the inventory!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest, nullptr);
	}

	HandleRequestCompleted(CurrentRequest, ItemsData[0]);
}
#pragma optimize("", on)

void ATartarusPickup::HandleRequestCompleted(const FPickupRequestInfo* const CompletedRequest, const UTartarusItem* const ItemData)
{
	if (!CompletedRequest)
	{
		return;
	}

	if (IsValid(ItemData))
	{
		Destroy();
	}

	PickupRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma endregion