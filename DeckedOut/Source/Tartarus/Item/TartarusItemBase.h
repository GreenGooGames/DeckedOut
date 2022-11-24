// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TartarusItemBase.generated.h"

UCLASS()
class TARTARUS_API ATartarusItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusItemBase();

	void SetItemId(const int32 ItemReferenceId) { ItemId = ItemReferenceId; }
	int32 GetItemId() { return ItemId; }

protected:
	// Number of stack this item contain.
	UPROPERTY(EditDefaultsOnly)
		int32 StackSize = 1;

private:
	int32 ItemId = -1;

};
