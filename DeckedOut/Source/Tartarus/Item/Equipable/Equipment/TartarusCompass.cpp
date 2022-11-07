// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/Equipment/TartarusCompass.h"

#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"

// TEMP
#include "Item/Persistent/TartarusTreasureChest.h"
// END TEMP

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#endif

ATartarusCompass::ATartarusCompass()
{
#if WITH_EDITORONLY_DATA
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(FName("ArrowComponent"), false);
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}

#if WITH_EDITORONLY_DATA
void ATartarusCompass::Tick(float DeltaSeconds)
{
	// Calculate the look at angle from the Arrow Component position to the target.
	const FRotator LookatRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

	// Set the Arrow component rotation.
	ArrowComponent->SetWorldRotation(LookatRotation);
}
#endif

void ATartarusCompass::OnEquipped()
{
	Super::OnEquipped();

	// TEMP
	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();

	if (!TreasureSubsystem)
	{
		return;
	}

	FSpawnAndLinkRequestCompletedEvent OnRequestCompleted;

	TreasureSubsystem->AsyncRequestSpawnedAndLinkTreasure(this, OnRequestCompleted);
	// END TEMP
}
