// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureSpawn.h"

#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"
#include "Logging/TartarusLogChannels.h"

#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#endif

// Sets default values
ATartarusTreasureSpawn::ATartarusTreasureSpawn()
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
void ATartarusTreasureSpawn::BeginPlay()
{
	Super::BeginPlay();
	
	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();

	if (!IsValid(TreasureSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to find UTartarusTreasureSubsystem in world!"), *FString(__FUNCTION__));
		return;
	}

	const FTransform& Transform = GetTransform();
	TreasureSubsystem->RegisterSpawnTransform(Transform);

	// The spawn is registered, no need to keep this allive.
	Destroy();
}

