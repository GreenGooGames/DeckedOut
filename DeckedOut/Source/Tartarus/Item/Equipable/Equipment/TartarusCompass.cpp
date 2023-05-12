// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipable/Equipment/TartarusCompass.h"

#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"
#include "Item/Equipable/TartarusEquipableManager.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerCharacter.h"
#include "Engine/World.h"

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

#pragma region EquipableInterface
void ATartarusCompass::OnEquipped(AActor* const EquippedActor)
{
	Super::OnEquipped(EquippedActor);

	const UTartarusTreasureSubsystem*const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();
	if (!IsValid(TreasureSubsystem))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to set target location: No treasureSubsytem found!"), *FString(__FUNCTION__));
		return;
	}

	const ATartarusPlayerCharacter* const PlayerCharacter = Cast<ATartarusPlayerCharacter>(EquippedActor);
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to set target location: Not equipped to a player!"), *FString(__FUNCTION__));
		return;
	}

	const UTartarusEquipableManager* const EquipableManager = PlayerCharacter->GetEquipableManager();
	if (!IsValid(EquipableManager))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to set target location: No equipable manager on the player, how did this get called?"), *FString(__FUNCTION__));
		return;
	}

	const FEquipmentInfo* const EquippableInfo = EquipableManager->FindEquippedItem(this);
	if (!EquippableInfo)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to set target location: This item is not equipped, how did this get called?"), *FString(__FUNCTION__));
		return;
	}

	TargetLocation = TreasureSubsystem->GetTreasureLocation(EquippableInfo->GetInventoryStackId());
}
#pragma endregion
