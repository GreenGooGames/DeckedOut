// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DeckedOut/Components/Interaction/InteractableInterface.h"
#include "DeckedOut/World/Items/CompassTargetInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreasureChest.generated.h"

class ULootComponent;
class UItemDataAsset;

UCLASS(Blueprintable)
class DECKEDOUT_API ATreasureChest : public AActor, public IInteractableInterface, public ICompassTargetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureChest();

protected:
	// The visual representation of the Item in the world.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULootComponent> LootComponent = nullptr;


	// [Koen Goossens] TODO: This should be a gameplay tag as that will be easier to manage.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UItemDataAsset>> CompatibleKeys;

private:
	const int32 NumKeysRequired = 1;

#pragma region IInteractableInterface
public:
	virtual bool IsInteractable() const override;
	virtual bool StartInteraction(const TObjectPtr<AController> InstigatorController) override;
#pragma endregion

#pragma region ICompassTargetInterface

#pragma endregion
};
