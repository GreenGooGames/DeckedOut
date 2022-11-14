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

	void SetReferenceId(const int32 ItemReferenceId) { ReferenceId = ItemReferenceId; }
	int32 GetReferenceId() { return ReferenceId; }

private:
	int32 ReferenceId = -1;

};
