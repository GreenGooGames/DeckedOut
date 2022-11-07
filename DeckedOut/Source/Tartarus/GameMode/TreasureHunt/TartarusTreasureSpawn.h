// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TartarusTreasureSpawn.generated.h"

UCLASS()
class TARTARUS_API ATartarusTreasureSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusTreasureSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	/** Normal editor sprite. */
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<class UBillboardComponent> Sprite;
#endif
};
