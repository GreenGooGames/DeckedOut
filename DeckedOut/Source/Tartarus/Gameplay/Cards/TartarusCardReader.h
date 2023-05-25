// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Objects/TartarusInteractablePopupActor.h"
#include "System/TartarusASyncLoadData.h"
#include "Item/Inventory/TartarusInventoryData.h"

#include "TartarusCardReader.generated.h"

class UTartarusInventoryComponent;
class ATartarusTreasureHuntGameState;
class UTartarusItem;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusCardReader : public ATartarusInteractablePopupActor
{
	GENERATED_BODY()
	
public:
	ATartarusCardReader();

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> Inventory = nullptr;

	virtual void BeginPlay() override;

#pragma region Modifiers
protected:
	void OnInventoryEntryChanged(EInventoryChanged ChangeType, FInventoryStackId StackId, int32 StackSize);
	void ApplyModifiersASync();
	void ApplyModifiers(const TMap<UTartarusItem*, int32>& CardsData);
#pragma endregion

#pragma region ASyncLoading
protected:
	struct FGetCardDataRequestInfo : public FASyncLoadRequest
	{
	public:
		FGetCardDataRequestInfo() {}
		FGetCardDataRequestInfo(TMap<FPrimaryAssetId, int32>& CardsToLoad);
		const TMap<FPrimaryAssetId, int32>& GetCardsToLoad() const { return CardIdsToCount; }
		TArray<FPrimaryAssetId> GetCardIdsToLoad() const;

	private:
		TMap<FPrimaryAssetId, int32> CardIdsToCount;
	};

	FGuid RequestCardsDataASync(const TArray<FPrimaryAssetId>& CardsToLoad);
	void HandleCardsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> CardsData);
	void HandleRequestCompleted(const FGetCardDataRequestInfo* const CompletedRequest);

private:
	TArray<FGetCardDataRequestInfo> GetCardDataRequests;
#pragma endregion
};
