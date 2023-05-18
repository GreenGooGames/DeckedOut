// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/TartarusHelpers.h"

#include "TartarusItemInstance.generated.h"

UCLASS()
class TARTARUS_API ATartarusItemInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusItemInstance();

	virtual void Initialize(const FPrimaryAssetId ItemReferenceId);
	FPrimaryAssetId GetItemId() { return ItemId; }

protected:
	// Number of stack this item contain.
	UPROPERTY(EditDefaultsOnly)
		int32 StackSize = 1;

private:
	FPrimaryAssetId ItemId = FTartarusHelpers::InvalidItemId;
};
