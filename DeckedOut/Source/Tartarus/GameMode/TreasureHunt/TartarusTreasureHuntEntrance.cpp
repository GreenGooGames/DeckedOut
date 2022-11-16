// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TreasureHunt/TartarusTreasureHuntEntrance.h"

#include "Components/BoxComponent.h"
#include "GameMode/TreasureHunt/TartarusTreasureHuntGameMode.h"
#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"
#include "Item/Equipable/TartarusEquipableManager.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"

ATartarusTreasureHuntEntrance::ATartarusTreasureHuntEntrance()
{
	// Close Trigger.
	CloseTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CLosing Trigger"));
	CloseTrigger->SetSimulatePhysics(false);
	CloseTrigger->SetCollisionProfileName(FName("Trigger"), false);
	CloseTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATartarusTreasureHuntEntrance::OnCloseTriggerOverlap);

	CloseTrigger->SetupAttachment(RootComponent);
}

void ATartarusTreasureHuntEntrance::HandleStateChanged(const EDoorState NewState, AController* const InstigatorController)
{
	Super::HandleStateChanged(NewState, InstigatorController);

	// ONly execute the behavior if the door opened, any other states skip.
	if (NewState != EDoorState::Open)
	{
		return;
	}

	// Is the game mode correct?
	ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

	if (!IsValid(GameMode))
	{
		return;
	}

	// [Koen Goossens] TODO: Should the gamemode on StartTreasureHunt not give the compass?
	// Gift the instigator a compass.
	UTartarusInventoryComponent* const Inventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();

	if (!IsValid(Inventory))
	{
		return;
	}

	FString ContextString = "";
	FItemTableRow* const ItemRow = GiftItemRow.GetRow<FItemTableRow>(ContextString);

	if (!ItemRow)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not retrieve the item row!"), __FUNCTION__);
		return;
	}

	// [Koen Goossens] TODO: Door should not open if the compass failed to be gifted.
	// [Koen Goossens] TODO: If the gift failed, should the compass be spawned into the world for later pickup?
	// [Koen Goossens] TODO: Magic number 1.
	FGuid StackId = Inventory->StoreItem(ItemRow->UniqueItemId, 1);

	// [Koen Goossens] TODO: If the compass cannot be gifted, then the interaction should fail and the door should not open.
	if (!StackId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to gift item: Could not store the item in the inventory!"), __FUNCTION__);
		return;
	}

	// Spawn a chest for this compass.
	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();

	if (!TreasureSubsystem)
	{
		return;
	}

	FSpawnAndLinkRequestCompletedEvent OnRequestCompleted;
	TreasureSubsystem->AsyncRequestSpawnAndLink(StackId, OnRequestCompleted);

	// Auto-equip the compass.
	UTartarusEquipableManager* const EquipableManager = InstigatorController->GetPawn()->FindComponentByClass<UTartarusEquipableManager>();

	if (!IsValid(EquipableManager))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Could not auto-equip the gift item: No equipableManager found!"), __FUNCTION__);
		return;
	}

	// [Koen Goossens] TODO: Try to equip in both left and right but by exposing as a var.
	bool bIsTryingToEquip = EquipableManager->ASyncRequestEquip(StackId, EEquipmentSlot::LeftHand);

	// Start the treasure hunt.
	GameMode->StartTreasureHunt();
}

void ATartarusTreasureHuntEntrance::OnCloseTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the OtherActor is the player.
	APawn* const OtherPawn = Cast<APawn>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (!OtherPawn->IsPlayerControlled())
	{
		return;
	}

	// Stop the treasure hunt.
	ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

	if (!IsValid(GameMode) || !GameMode->IsTreasureHuntInProgress())
	{
		return;
	}

	GameMode->StopTreasureHunt();

	// Close the door.
	EDoorState NewState = EDoorState::Closed;
	bool bHasStateChanged = ChangeState(NewState);

	if (bHasStateChanged)
	{
		HandleStateChanged(NewState, OtherPawn->GetController());
	}
}

#pragma region ITartarusInteractableTargetInterface
bool ATartarusTreasureHuntEntrance::IsInteractable() const
{
	bool bIsInteractable = Super::IsInteractable();

	ATartarusTreasureHuntGameMode* const GameMode = GetWorld()->GetAuthGameMode<ATartarusTreasureHuntGameMode>();

	if (!IsValid(GameMode))
	{
		return false;
	}

	bIsInteractable = bIsInteractable && !GameMode->IsTreasureHuntInProgress();

	return bIsInteractable;
}
#pragma endregion
