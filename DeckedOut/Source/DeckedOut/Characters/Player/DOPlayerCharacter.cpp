// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/Characters/Player/DOPlayerCharacter.h"

#include "DeckedOut/Characters/EquipmentManager.h"
#include "DeckedOut/Characters/Player/DOPlayerController.h"
#include "DeckedOut/Components/Inventory/InventoryComponent.h"
#include "DeckedOut/World/Items/ItemManagerSubsystem.h"

// Sets default values
ADOPlayerCharacter::ADOPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	EquipmentManager = CreateDefaultSubobject<UEquipmentManager>(FName("EquipmentManager"), false);
}

// Called to bind functionality to input
void ADOPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsValid(PlayerInputComponent))
	{
		PlayerInputComponent->BindAction("DEBUG_EquipCompass", EInputEvent::IE_Pressed, this, &ADOPlayerCharacter::DEBUG_EquipCompass);
		PlayerInputComponent->BindAction("DEBUG_UnequipCompass", EInputEvent::IE_Pressed, this, &ADOPlayerCharacter::DEBUG_UnequipCompass);
	}
}

void ADOPlayerCharacter::DEBUG_EquipCompass()
{
	// [Koen Goossens] TODO: Add a check to see if the compass is in the inventory and can be equiped.
	UItemManagerSubsystem* const ItemManager = GetWorld()->GetSubsystem<UItemManagerSubsystem>();

	if (!IsValid(ItemManager))
	{
		return;
	}

	// Get a compass from the inventory.
	ADOPlayerController* const PlayerController = Cast<ADOPlayerController>(GetController());

	UInventoryComponent* const Inventory = PlayerController->GetInventoryComponent();
	int32 NumRetrievedCompasses = 0;
	TMap<FString, ItemUniqueDataType> UniqueItemData;

	if (!Inventory->RetrieveItems(DEBUG_CompassID, 1, NumRetrievedCompasses, UniqueItemData))
	{
		return;
	}

	// Spawn the compass in the world.
	TSoftObjectPtr<AActor> Compass = ItemManager->SpawnItem(DEBUG_CompassID, GetActorTransform(), UniqueItemData);
	
	if (IsValid(Compass.Get()))
	{
		// Equip the compass.
		EquipmentManager->Equip(Compass, EEquipmentSlot::LeftHand);
		Compass->SetActorRelativeLocation(FVector::ZeroVector);
	}
}

void ADOPlayerCharacter::DEBUG_UnequipCompass()
{
	// Get the item from the equipment manager.
	const TMap<EEquipmentSlot, TSoftObjectPtr<AActor>>& EquippedActors = EquipmentManager->GetEquipment();

	if (EquippedActors.Num() <= 0)
	{
		return;
	}

	if (!EquippedActors.Contains(EEquipmentSlot::LeftHand))
	{
		return;
	}

	TSoftObjectPtr<AActor> Compass = EquippedActors[EEquipmentSlot::LeftHand];
	
	if (!EquipmentManager->Unequip(Compass))
	{
		return;
	}

	// Add the item to the inventory.
	ADOPlayerController* const PlayerController = Cast<ADOPlayerController>(GetController());
	AItem* const CompassRaw = Cast<AItem>(Compass.Get());

	FItemData ItemData = CompassRaw->GetItemData();
	TMap<FString, ItemUniqueDataType> UniqueItemData;
	CompassRaw->ReadUniqueData(UniqueItemData);

	UInventoryComponent* const Inventory = PlayerController->GetInventoryComponent();
	Inventory->StoreItem(ItemData, UniqueItemData, 1);

	// Destroy the item.
	UItemManagerSubsystem* const ItemManagerSubsystem = GetWorld()->GetSubsystem< UItemManagerSubsystem>();

	if (IsValid(ItemManagerSubsystem))
	{
		ItemManagerSubsystem->DespawnItem(CompassRaw);
	}
}

