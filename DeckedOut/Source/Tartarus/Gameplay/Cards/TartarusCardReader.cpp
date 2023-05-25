// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cards/TartarusCardReader.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameState.h"
#include "Gameplay/Cards/TartarusCard.h"
#include "Engine/World.h"
#include "Logging/TartarusLogChannels.h"

#if WITH_EDITOR
#include "Engine/Engine.h"
#endif

ATartarusCardReader::ATartarusCardReader()
{
	Inventory = CreateDefaultSubobject<UTartarusInventoryComponent>("InventoryComponent");
}

void ATartarusCardReader::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Inventory))
	{
		Inventory->OnInventoryEntryUpdated().AddUObject(this, &ATartarusCardReader::OnInventoryEntryChanged);
	}
}

#pragma region Modifers
void ATartarusCardReader::OnInventoryEntryChanged(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize)
{
	ApplyModifiersASync();
}

void ATartarusCardReader::ApplyModifiersASync()
{
	// Reset all modifiers.
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to apply modifiers: Game State is invalid!"), *FString(__FUNCTION__));
		return;
	}

	GameState->ResetGameModifiers();

	// Get the inventory of the instigator.
	if (!IsValid(Inventory))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to apply modifiers: No inventory to search for cards!"), *FString(__FUNCTION__));
		return;
	}

	// Get all cards from the inventory.
	TArray<FInventoryStack> InventoryCards = Inventory->GetOverview(Inventory->GetSubInventoryIds()[0]);

	// Retrieve the Id of each card in the inventory.
	TMap<FPrimaryAssetId, int32> CardsToLoad;
	for (const FInventoryStack& InventoryStack : InventoryCards)
	{
		if (!InventoryStack.GetEntryId().IsValid())
		{
			continue;
		}

		// Since the inventory of the CardReader is non-stackable verify if a card already exists.
		if (CardsToLoad.Contains(InventoryStack.GetEntryId()))
		{
			CardsToLoad[InventoryStack.GetEntryId()] += InventoryStack.StackSize;
		}
		else
		{
			CardsToLoad.Add(InventoryStack.GetEntryId(), InventoryStack.StackSize);
		}
	}

	// No need to continue if no cards are in the inventory.
	if (CardsToLoad.IsEmpty())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Unable to apply modifiers: No cards found in the inventory!"), *FString(__FUNCTION__));
		return;
	}

	FGetCardDataRequestInfo RequestInfo = FGetCardDataRequestInfo(CardsToLoad);

	// AsyncLoad the data of all the cards.
	const FGuid RequestId = RequestCardsDataASync(RequestInfo.GetCardIdsToLoad());
	if (!RequestId.IsValid())
	{
		return;
	}

	RequestInfo.SetASyncLoadRequestId(RequestId);
	GetCardDataRequests.Add(RequestInfo);
}

void ATartarusCardReader::ApplyModifiers(const TMap<UTartarusItem*, int32>& CardsData)
{
	ATartarusTreasureHuntGameState* const GameState = GetWorld()->GetGameState<ATartarusTreasureHuntGameState>();
	if (!IsValid(GameState))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to apply modifiers: Game State was invalid!"), *FString(__FUNCTION__));
		return;
	}

	for (const auto CardData : CardsData)
	{
		const UTartarusCard* const Card = Cast<UTartarusCard>(CardData.Key);
		if (!IsValid(Card))
		{
			continue;
		}

		for (const auto& Modifier : Card->Modifiers)
		{
			GameState->EditGameModifier(Modifier.Key, Modifier.Value * CardData.Value);

#if WITH_EDITOR
			FString EnumToString = "";

			const UEnum* const ModiferTypePtr = FindObject<UEnum>(FTopLevelAssetPath(TEXT("/Script/Tartarus.EGameModifier")), true);
			if (IsValid(ModiferTypePtr))
			{
				EnumToString = ModiferTypePtr->GetNameStringByValue(static_cast<int64>(Modifier.Key));
			}

			UE_LOG(LogTartarus, Warning, TEXT("%s: Applied modifer %s with value %d!"), *FString(__FUNCTION__), *EnumToString, Modifier.Value * CardData.Value);

			FString Text = FString("Applied modifer ") + EnumToString + FString(" with value ") + FString::SanitizeFloat(Modifier.Value * CardData.Value);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, Text);
#endif
		}
	}
}
#pragma endregion

#pragma region ASyncLoading
ATartarusCardReader::FGetCardDataRequestInfo::FGetCardDataRequestInfo(TMap<FPrimaryAssetId, int32>& CardsToLoad)
{
	RequestId = FGuid::NewGuid();
	CardIdsToCount = CardsToLoad;
}

TArray<FPrimaryAssetId> ATartarusCardReader::FGetCardDataRequestInfo::GetCardIdsToLoad() const
{
	TArray<FPrimaryAssetId> CardIds;
	for (const auto& Data : CardIdsToCount)
	{
		CardIds.Add(Data.Key);
	}
	return CardIds;
}

FGuid ATartarusCardReader::RequestCardsDataASync(const TArray<FPrimaryAssetId>& CardsToLoad)
{
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();
	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create a request to retrieve the card data: ItemSubsystem is not found!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FGetItemDataRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusCardReader::HandleCardsDataLoaded);

	const FGuid RequestId = ItemSubsystem->AsyncRequestGetItemsData(CardsToLoad, OnRequestCompleted);
	if (!RequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Unable to create a request to retrieve the card data: Failed to create a request!"), *FString(__FUNCTION__));
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

	TMap<UTartarusItem*, int32> LoadedCards;
	for (UTartarusItem* const Card : CardsData)
	{
		LoadedCards.Add(Card, CurrentRequest->GetCardsToLoad()[Card->GetPrimaryAssetId()]);
	}

	ApplyModifiers(LoadedCards);

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
