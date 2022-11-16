// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusTreasureChest.generated.h"

class ATartarusCompass;
class ATartarusItemBase;
class UTartarusLootComponent;

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
	// Links a key to exclusivly open this chest.
	void LinkKey(FGuid KeyStackId) { KeyInventoryStackId = KeyStackId; }

	// Event fired when this chest is looted.
	FLootedEvent& OnLooted() { return LootedEvent; }

	/*
	* Retrieve the key id that is linked to this treasure.
	* Return: the Id of the stack that holds the key in the player inventory.
	*/
	FGuid GetLinkedKey() { return KeyInventoryStackId; }

protected:
	// The component responsible for awarding loot.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusLootComponent> LootComponent = nullptr;
	
	// Fired when the loot is spawned into the world.
	void HandleLootDropped(FGuid RequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedLoot);

private:
	FLootedEvent LootedEvent = FLootedEvent();
	FGuid KeyInventoryStackId = FGuid();

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion
};
