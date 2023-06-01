// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/System/TartarusAISpawnPoint.h"

#include "AI/System/TartarusAISubsystem.h"
#include "Engine/World.h"
#include "Logging/TartarusLogChannels.h"


#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#endif

// Sets default values
ATartarusAISpawnPoint::ATartarusAISpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

#if WITH_EDITORONLY_DATA
	Sprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (IsValid(Sprite))
	{
		Sprite->SetupAttachment(RootComponent);

		if (!IsRunningCommandlet())
		{
			Sprite->bHiddenInGame = true;
			Sprite->SetupAttachment(RootComponent);
			Sprite->SetUsingAbsoluteScale(true);
			Sprite->bIsScreenSizeScaled = true;
		}
	}
#endif
}

// Called when the game starts or when spawned
void ATartarusAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	UTartarusAISubsystem* const AISubsystem = GetWorld()->GetSubsystem<UTartarusAISubsystem>();
	if (!IsValid(AISubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to register AI spawn point: Unable to fins the AISubsystem!"), *FString(__FUNCTION__));
		return;
	}

	AISubsystem->RegisterSpawnLocation(PointId, GetActorTransform());

	Destroy(); // This has served its purpose!
}


