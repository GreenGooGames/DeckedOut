// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"
#include "System/TartarusAsyncLoadData.h"
#include "GameplayTagContainer.h"

#include "TartarusCardReader.generated.h"

class UBoxComponent;
class UTartarusItem;
class ATartarusTreasureHuntGameState;
class UTartarusWidgetComponent;

#pragma region ASyncLoading
USTRUCT()
struct FGetCardDataRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FGetCardDataRequestInfo() {}
	FGetCardDataRequestInfo(ATartarusTreasureHuntGameState* const TargetGameState, TMap<FPrimaryAssetId, int32>& CardsToLoad);

	ATartarusTreasureHuntGameState* GetGameState() const { return GameState.Get(); }
	const TMap<FPrimaryAssetId, int32>& GetCardsToLoad() const { return CardIdsToCount; }
	TArray<FPrimaryAssetId> GetCardIdsToLoad() const;

private:
	TMap<FPrimaryAssetId, int32> CardIdsToCount;
	TWeakObjectPtr<ATartarusTreasureHuntGameState> GameState = nullptr;
};
#pragma endregion

UCLASS()
class TARTARUS_API ATartarusCardReader : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusCardReader();

protected:
	// Visual representation of the door.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Collision on this object.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UBoxComponent> Collider = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess=true))
		FGameplayTag CardInventoryId = FGameplayTag::EmptyTag;

#pragma region ITartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
	virtual void DisableInteraction() override;
	virtual bool ToggleInteractionPrompt(const bool bShowPrompt);

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusWidgetComponent> InteractionWidgetComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		FText InteractionText = FText();

	void CreateInteractionWidgetComponent();
#pragma endregion

#pragma region ASyncLoading
protected:
	FGuid RequestCardsDataASync(const TArray<FPrimaryAssetId>& CardsToLoad);
	void HandleCardsDataLoaded(FGuid ASyncLoadRequestId, TArray<UTartarusItem*> CardsData);
	void HandleRequestCompleted(const FGetCardDataRequestInfo* const CompletedRequest);

private:
	TArray<FGetCardDataRequestInfo> GetCardDataRequests;
#pragma endregion
};
