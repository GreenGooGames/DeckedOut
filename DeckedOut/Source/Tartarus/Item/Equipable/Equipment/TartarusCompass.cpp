// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/Equipment/TartarusCompass.h"

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

	if (IsValid(ArrowComponent))
	{
		ArrowComponent->SetupAttachment(RootComponent);
	}
#endif
}

#if WITH_EDITORONLY_DATA
void ATartarusCompass::Tick(float DeltaSeconds)
{
	if (IsValid(ArrowComponent))
	{
		// Calculate the look at angle from the Arrow Component position to the target.
		const FRotator LookatRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

		// Set the Arrow component rotation.
		ArrowComponent->SetWorldRotation(LookatRotation);
	}
}
#endif

// [Koen Goossens] TODO: OnEquip ASk the TreasureSubssytem for the targetlocation using the invnetoryStackId of this item.
