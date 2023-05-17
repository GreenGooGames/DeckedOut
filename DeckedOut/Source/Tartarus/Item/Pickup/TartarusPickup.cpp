// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup/TartarusPickup.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Controller.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItem.h"
#include "Logging/TartarusLogChannels.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TartarusPickup.h"
#include "UI/Gameplay/TartarusInteractionWidget.h"
#include "UI/Foundation/TartarusWidgetComponent.h"

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

	// Interaction
	CreateInteractionWidgetComponent();
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
	return WasRecentlyRendered() && bIsInteractable;
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

void ATartarusPickup::DisableInteraction()
{
	bIsInteractable = false;
}

bool ATartarusPickup::ToggleInteractionPrompt(const bool bShowPrompt)
{
	if (!IsValid(InteractionWidgetComponent))
	{
		return false;
	}

	InteractionWidgetComponent->SetHiddenInGame(!bShowPrompt);

	return true;
}

void ATartarusPickup::CreateInteractionWidgetComponent()
{
	// Create and setup the Widget Component to display the interaction prompt.
	InteractionWidgetComponent = CreateDefaultSubobject<UTartarusWidgetComponent>(TEXT("InteractionWidget"));
	if (!IsValid(InteractionWidgetComponent))
	{
		return;
	}

	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidgetComponent->SetSimulatePhysics(false);
	InteractionWidgetComponent->SetHiddenInGame(true);

	// Setup the Widget to display with the correct data.
	InteractionWidgetComponent->OnWidgetCreatedEvent().AddLambda([&](UTartarusWidgetComponent* WidgetComponent)
		{
			// Setup the Widget to display with the correct data.
			UTartarusInteractionWidget* const InteractionWidget = Cast<UTartarusInteractionWidget>(WidgetComponent->GetWidget());
			if (!IsValid(InteractionWidget))
			{
				return;
			}

			// TODO: Request the Item data, and get the name of the item, then set the text
			//InteractionWidget->SetText(InteractionText);
			InteractionWidget->UpdateInputActionWidget();
		});
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

void ATartarusPickup::HandleRequestCompleted(const FPickupRequestInfo* const CompletedRequest, const UTartarusItem* const ItemData)
{
	if (!CompletedRequest)
	{
		return;
	}

	PickupRequests.RemoveSingleSwap(*CompletedRequest);

	// Despawn the item if it is managed by the item manager, otherwise it is up to the owner to despawn this.
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to despawn pickup: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return;
	}

	const bool bIsDespawned = ItemSubsystem->DespawnItem(this);
	if (!bIsDespawned)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to despawn pickup: despawn failed!"), *FString(__FUNCTION__));
		return;
	}
}
#pragma endregion