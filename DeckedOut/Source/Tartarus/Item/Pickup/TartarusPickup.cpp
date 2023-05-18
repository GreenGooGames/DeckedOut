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
FPickupRequestInfo::FPickupRequestInfo(UTartarusInventoryComponent* const InstigatorInventory, const bool bIsPickupRequest)
{
	RequestId = FGuid::NewGuid();
	
	Inventory = InstigatorInventory;
	bShouldPickup = bIsPickupRequest;
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

void ATartarusPickup::Initialize(const FPrimaryAssetId ItemReferenceId)
{
	Super::Initialize(ItemReferenceId);

	// Create an Async request to get the appopriate data for this object.
	const FGuid RequestId = RequestItemData();
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Failed to create a request to retrieve the item data!"), *FString(__FUNCTION__));
		return;
	}

	FPickupRequestInfo PickupRequest = FPickupRequestInfo(nullptr, false);
	PickupRequest.SetASyncLoadRequestId(RequestId);
	PickupRequests.Add(PickupRequest);
}

bool ATartarusPickup::PickupItemAsync(const TObjectPtr<AController> InstigatorController)
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

	FPickupRequestInfo PickupRequest = FPickupRequestInfo(InstigatorInventory, true);
	PickupRequest.SetASyncLoadRequestId(RequestId);
	PickupRequests.Add(PickupRequest);

	return true;
}

bool ATartarusPickup::PickupItem(UTartarusInventoryComponent* const Inventory, const UTartarusItem* const Item)
{
	// Validate if all data and environment.
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Inventory no longer exists after interaction!"), *FString(__FUNCTION__));
		return false;
	}

	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to despawn pickup: Item subsystem is invalid!"), *FString(__FUNCTION__));
		return false;
	}

	// Pickup the item by storing it in the inventory.
	const bool bIsStored = Inventory->StoreEntry(Item, StackSize).IsValid();
	if (!bIsStored)
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Interaction Failed: Could not store the item in the inventory!"), *FString(__FUNCTION__));
		return false;
	}

	// Despawn the item if it is managed by the item manager, otherwise it is up to the owner to despawn this.
	const bool bIsDespawned = ItemSubsystem->DespawnItem(this);
	if (!bIsDespawned)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to despawn pickup: despawn failed!"), *FString(__FUNCTION__));
		return false;
	}

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

	const bool bSuccess = PickupItemAsync(InstigatorController);

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
}

bool ATartarusPickup::SetupInteractionPrompt(const UTartarusItem* const Item)
{
	UTartarusInteractionWidget* const InteractionWidget = Cast<UTartarusInteractionWidget>(InteractionWidgetComponent->GetWidget());
	if (IsValid(InteractionWidget))
	{
		InteractionWidget->SetText(Item->Name);
		InteractionWidget->UpdateInputActionWidget();
	}
	else
	{
		// the widget doesn't exist yet, register a callback for when it does.
		InteractionWidgetComponent->OnWidgetCreatedEvent().AddLambda([&](UTartarusWidgetComponent* WidgetComponent)
			{
				// Setup the Widget to display with the correct data.
				UTartarusInteractionWidget* const InteractionWidget = Cast<UTartarusInteractionWidget>(WidgetComponent->GetWidget());
				if (!IsValid(InteractionWidget))
				{
					return;
				}

				InteractionWidget->SetText(Item->Name);
				InteractionWidget->UpdateInputActionWidget();
			});
	}

	return true;
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
		return HandleRequestCompleted(CurrentRequest);
	}

	if (ItemsData.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Pickup: No Item data is loaded!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	if (CurrentRequest->ShouldPickup())
	{
		if (!PickupItem(CurrentRequest->GetInventory(), ItemsData[0]))
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Pickup: Unable to pickup the item!"), *FString(__FUNCTION__));
		}
	}
	else
	{
		if (!SetupInteractionPrompt(ItemsData[0]))
		{
			UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Setup Interaction Prompt!"), *FString(__FUNCTION__));
		}
	}

	HandleRequestCompleted(CurrentRequest);
}

void ATartarusPickup::HandleRequestCompleted(const FPickupRequestInfo* const CompletedRequest)
{
	if (!CompletedRequest)
	{
		return;
	}

	PickupRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma endregion
