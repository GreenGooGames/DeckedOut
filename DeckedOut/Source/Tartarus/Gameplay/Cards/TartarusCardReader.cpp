// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cards/TartarusCardReader.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Cards/TartarusCard.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Logging/TartarusLogChannels.h"

#pragma region ASyncLoading
FGetCardDataRequestInfo::FGetCardDataRequestInfo(ATartarusTreasureHuntGameState* const TargetGameState, TMap<FPrimaryAssetId, int32>& CardsToLoad)
{
	RequestId = FGuid::NewGuid();
	GameState = TargetGameState;
	CardIdsToCount = CardsToLoad;
}

TArray<FPrimaryAssetId> FGetCardDataRequestInfo::GetCardIdsToLoad() const
{
	TArray<FPrimaryAssetId> CardIds;

	for (const auto& Data : CardIdsToCount)
	{
		CardIds.Add(Data.Key);
	}

	return CardIds;
}
#pragma endregion

// Sets default values
ATartarusCardReader::ATartarusCardReader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collider
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetSimulatePhysics(false);
	Collider->SetCollisionProfileName(FName("Trigger"), false);

	SetRootComponent(Collider);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetupAttachment(Collider);
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusCardReader::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusCardReader::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	// Reset all modifiers.
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	GameState->ResetGameModifiers();

	// Get the inventory of the instigator.
	const UTartarusInventoryComponent* const InstigatorInventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	// Get all cards from the player inventory.
	TArray<FInventoryStack> InventoryCards = InstigatorInventory->GetOverview(EInventoryType::Card);

	// TODO: Show a UI to have the player select cards from their inventory and only activate a few select ones.
	// Retrieve the effect of each card in the inventory.
	TMap<FPrimaryAssetId, int32> CardsToLoad;

	for (const FInventoryStack& InventoryStack : InventoryCards)
	{
		if (!InventoryStack.GetEntryId().IsValid())
		{
			continue;
		}

		CardsToLoad.Add(InventoryStack.GetEntryId(), InventoryStack.StackSize);
	}

	// No need to continue if no cards are in the inventory.
	if (CardsToLoad.IsEmpty())
	{
		return false;
	}

	FGetCardDataRequestInfo RequestInfo = FGetCardDataRequestInfo(GameState, CardsToLoad);

	// AsyncLoad the data of all the cards.
	const FGuid RequestId = RequestCardsDataASync(RequestInfo.GetCardIdsToLoad());
	if (!RequestId.IsValid())
	{
		return false;
	}

	RequestInfo.SetASyncLoadRequestId(RequestId);
	GetCardDataRequests.Add(RequestInfo);

	return true;
}

void ATartarusCardReader::DisableInteraction()
{
	check(false);
	UE_LOG(LogTartarus, Warning, TEXT("%s: Not Implemented!"), *FString(__FUNCTION__));
}
#pragma endregion

#pragma region ASyncLoading
FGuid ATartarusCardReader::RequestCardsDataASync(const TArray<FPrimaryAssetId>& CardsToLoad)
{
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request Failed: ItemSubsystem is not found!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusCardReader::HandleCardsDataLoaded);

	const FGuid RequestId = ItemSubsystem->AsyncRequestGetItemsData(CardsToLoad, OnRequestCompleted);
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Request Failed: Failed to create a request!"), *FString(__FUNCTION__));
		return FGuid();
	}

	return RequestId;
}

void ATartarusCardReader::HandleCardsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> CardsData)
{
	// Get the request that is being handled.
	FGetCardDataRequestInfo* const CurrentRequest = GetCardDataRequests.FindByPredicate([&ASyncLoadRequestId](const FGetCardDataRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Get Card Data: Could not find the request!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	if (CardsData.IsEmpty())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Get Card Data: No Card data is loaded!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	ATartarusTreasureHuntGameState* const GameState = CurrentRequest->GetGameState();
	if (!IsValid(GameState))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to Get Card Data: Game State was invalidated!"), *FString(__FUNCTION__));
		return HandleRequestCompleted(CurrentRequest);
	}

	for (const UTartarusItem* const CardData : CardsData)
	{
		const UTartarusCard* const Card = Cast<UTartarusCard>(CardData);
		if (!IsValid(Card))
		{
			continue;
		}

		for (const auto& Modifier : Card->Modifiers)
		{
			GameState->EditGameModifier(Modifier.Key, Modifier.Value * CurrentRequest->GetCardsToLoad()[CardData->GetPrimaryAssetId()]);

#if WITH_EDITOR
			FString EnumToString = "";
			const UEnum* const ModiferTypePtr = FindObject<UEnum>(FTopLevelAssetPath(TEXT("/Script/Tartarus.EGameModifier")), true);
			if (IsValid(ModiferTypePtr))
			{
				EnumToString = ModiferTypePtr->GetNameStringByValue(static_cast<int64>(Modifier.Key));
			}

			UE_LOG(LogTartarus, Warning, TEXT("%s: Applied modifer %s with value %d!"), *FString(__FUNCTION__), *EnumToString, Modifier.Value * CurrentRequest->GetCardsToLoad()[CardData->GetPrimaryAssetId()]);

			FString Text = FString("Applied modifer ") + EnumToString + FString(" with value ") + FString::SanitizeFloat(Modifier.Value * CurrentRequest->GetCardsToLoad()[CardData->GetPrimaryAssetId()]);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, Text);
#endif
		}
	}

	HandleRequestCompleted(CurrentRequest);
}

void ATartarusCardReader::HandleRequestCompleted(const FGetCardDataRequestInfo* const CompletedRequest)
{
	if (!CompletedRequest)
	{
		return;
	}

	GetCardDataRequests.RemoveSingleSwap(*CompletedRequest);
}
#pragma endregion
