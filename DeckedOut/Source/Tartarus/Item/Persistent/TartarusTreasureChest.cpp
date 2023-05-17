// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Persistent/TartarusTreasureChest.h"

#include "Audio/TartarusNoiseSourceComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameMode/TreasureHunt/TartarusTreasureSubsystem.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/Loot/TartarusLootComponent.h"
#include "Player/TartarusPlayerCharacter.h"
#include "logging/TartarusLogChannels.h"
#include "TartarusTreasureChest.h"
#include "UI/Gameplay/TartarusInteractionWidget.h"
#include "UI/Foundation/TartarusWidgetComponent.h"

ATartarusTreasureChest::ATartarusTreasureChest()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Interaction
	CreateInteractionWidgetComponent();

	LootComponent = CreateDefaultSubobject<UTartarusLootComponent>(TEXT("LootComponent"));
	NoiseSourceComponent = CreateDefaultSubobject<UTartarusNoiseSourceComponent>(TEXT("Noise Source Component"));
}

void ATartarusTreasureChest::HandleLootDropped(FGuid RequestId, TArray<TWeakObjectPtr<ATartarusItemInstance>> SpawnedLoot)
{
	LootedEvent.Broadcast(this);
}

#pragma region TartarusInteractableTargetInterface
bool ATartarusTreasureChest::IsInteractable() const
{
	return WasRecentlyRendered();
}

bool ATartarusTreasureChest::StartInteraction(const TObjectPtr<AController> InstigatorController)
{
	ATartarusPlayerCharacter* const Player = Cast<ATartarusPlayerCharacter>(InstigatorController->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Interaction failed: Instigator was not a player!"), *FString(__FUNCTION__));
		return false;
	}

	// Remove the compass from the inventory.
	UTartarusInventoryComponent* const Inventory = InstigatorController->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(Inventory))
	{
		return false;
	}

	UTartarusTreasureSubsystem* const TreasureSubsystem = GetWorld()->GetSubsystem<UTartarusTreasureSubsystem>();
	if (!IsValid(TreasureSubsystem))
	{
		return false;
	}

	const FInventoryStackId LinkedKeyInventoryStackId = TreasureSubsystem->GetTreasureKey(this);
	if (!LinkedKeyInventoryStackId.IsValid())
	{
		return false;
	}

	// Retrieve the number of required keys, fail if there are not enough keys.
	const bool bHasRetrievedItem = Inventory->RetrieveEntry(LinkedKeyInventoryStackId, NumKeysRequired);
	if (!bHasRetrievedItem)
	{
		return false;
	}

	// Droploot.
	FDropLootRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusTreasureChest::HandleLootDropped);

	LootComponent->AsyncRequestDropLoot(GetActorTransform(), OnRequestCompleted);

	// Play sound of the chest opening.
	NoiseSourceComponent->GenerateNoise(LootingSound, GetActorLocation());

	return true;
}

void ATartarusTreasureChest::DisableInteraction()
{
	check(false);
	UE_LOG(LogTartarus, Warning, TEXT("%s: Not Implemented!"), *FString(__FUNCTION__));
}

bool ATartarusTreasureChest::ToggleInteractionPrompt(const bool bShowPrompt)
{
	if (!IsValid(InteractionWidgetComponent))
	{
		return false;
	}

	InteractionWidgetComponent->SetHiddenInGame(!bShowPrompt);

	return true;
}

void ATartarusTreasureChest::CreateInteractionWidgetComponent()
{
	// Create and setup the Widget Component to display the interaction prompt.
	InteractionWidgetComponent = CreateDefaultSubobject<UTartarusWidgetComponent>(TEXT("InteractionWidget"));
	if (!IsValid(InteractionWidgetComponent))
	{
		return;
	}

	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidgetComponent->SetSimulatePhysics(false); 
	InteractionWidgetComponent->SetHiddenInGame(true);

	InteractionWidgetComponent->OnWidgetCreatedEvent().AddLambda([&](UTartarusWidgetComponent* WidgetComponent)
		{
			// Setup the Widget to display with the correct data.
			UTartarusInteractionWidget* const InteractionWidget = Cast<UTartarusInteractionWidget>(WidgetComponent->GetWidget());
			if (!IsValid(InteractionWidget))
			{
				return;
			}

			InteractionWidget->SetText(InteractionText);
			InteractionWidget->UpdateInputActionWidget();
		});
}
#pragma endregion