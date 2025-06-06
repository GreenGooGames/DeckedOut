// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Audio/TartarusSoundData.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusTreasureChest.generated.h"

class ATartarusCompass;
class ATartarusItemInstance;
class UTartarusLootComponent;
class UTartarusNoiseSourceComponent;
class UTartarusInteractionWidget;
class UTartarusWidgetComponent;
class UGleipnirLootComponent;

DECLARE_EVENT_OneParam(ATartarusTreasureChest, FLootedEvent, ATartarusTreasureChest* const /*LootedTreasure*/);

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusTreasureChest : public AActor, public ITartarusInteractableTargetInterface
{
	GENERATED_BODY()
	
public:
	ATartarusTreasureChest();

public:
	// Event fired when this chest is looted.
	FLootedEvent& OnLooted() { return LootedEvent; }

protected:
	// How many keys are required to open this chest. (Only works with non-Unique keys)
	int32 NumKeysRequired = 1;

	// The component responsible for awarding loot.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusLootComponent> TartarusLootComponent = nullptr;	
	
	// The component responsible for awarding loot.
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGleipnirLootComponent> LootComponent = nullptr;
	
	// Fired when the loot is spawned into the world.
	void HandleLootDropped(FGuid RequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot);

private:
	FLootedEvent LootedEvent = FLootedEvent();

#pragma region TartarusInteractableTargetInterface
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

#pragma region Audio
protected:
	UPROPERTY(EditDefaultsOnly)
		FTartarusSound LootingSound = FTartarusSound();

	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusNoiseSourceComponent> NoiseSourceComponent = nullptr;
#pragma endregion
};
