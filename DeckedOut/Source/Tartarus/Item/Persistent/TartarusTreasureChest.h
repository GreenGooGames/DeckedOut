// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TartarusInteractableTargetInterface.h"

#include "TartarusTreasureChest.generated.h"

class ATartarusCompass;
class ATartarusItemBase;
class UTartarusLootComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLooted, ATartarusTreasureChest* const, LootedTreasure);

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
	// [Koen Goossens] TODO: This WeakPtr will be invalid once the Compass is stored in the inventory. Identify the hourglass through different means.
	void LinkCompass(TWeakObjectPtr<ATartarusCompass> Compass) { LinkedCompass = Compass; }

	FOnLooted OnLooted;

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusLootComponent> LootComponent = nullptr;
	
	TWeakObjectPtr<ATartarusCompass> LinkedCompass = nullptr;

	void HandleLootDropped(FGuid RequestId, TArray<TWeakObjectPtr<ATartarusItemBase>> SpawnedLoot);

#pragma region TartarusInteractableTargetInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion

};
