// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/World/Items/Compass.h"

#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACompass::ACompass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.3f;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(FName("ArrowComponent"), false);
	ArrowComponent->SetupAttachment(Mesh);
}

void ACompass::Tick(float DeltaSeconds)
{
	// Calculate the look at angle from the Arrow Component position to the target.
	const FRotator LookatRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

	// Set the Arrow component rotation.
	ArrowComponent->SetWorldRotation(LookatRotation);
}

TMap<FString, ItemUniqueDataType> ACompass::GenerateUniqueData()
{
	TMap<FString, ItemUniqueDataType> UniqueData;

	ItemUniqueDataType TargetLocationData;
	TargetLocationData.Set<FVector>(TargetLocation);
	UniqueData.Add(TargetLocationDataName, TargetLocationData);

	return UniqueData;
}

bool ACompass::WriteUniqueData(const TMap<FString, ItemUniqueDataType>& UniqueData)
{
	// [Koen Goossens] TODO: Print an error if the retrieved data was not of the expected type.
	const ItemUniqueDataType* const UniqueItemData = UniqueData.Find(TargetLocationDataName);

	if (UniqueItemData != nullptr)
	{
		const FVector* const ReadTargetLocation = UniqueItemData->TryGet<FVector>();
		TargetLocation = ReadTargetLocation != nullptr ? *ReadTargetLocation : FVector::ZeroVector;
	}

	return false;
}

#pragma region IEquipableInterface
void ACompass::OnEquip()
{
	SetupComponents(false);
}

void ACompass::OnUnequip()
{

}
#pragma endregion
