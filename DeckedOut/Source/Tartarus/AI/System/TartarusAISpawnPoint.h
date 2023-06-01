// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "TartarusAISpawnPoint.generated.h"

UCLASS()
class TARTARUS_API ATartarusAISpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATartarusAISpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true))
		FGameplayTag PointId = FGameplayTag::EmptyTag;

#if WITH_EDITORONLY_DATA
	/** Normal editor sprite. */
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<class UBillboardComponent> Sprite;
#endif
};
