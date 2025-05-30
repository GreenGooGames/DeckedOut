// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TartarusPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/TartarusInteractableSourceComponent.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"
#include "GameFramework/Pawn.h"
#include "Inventory/CorrbolgInventoryManagerComponent.h"

ATartarusPlayerController::ATartarusPlayerController()
{
	TartarusInventoryComponent = CreateDefaultSubobject<UTartarusInventoryComponent>("Tartarus Inventory component", false);
	CorrbolgInventoryComponent = CreateDefaultSubobject<UCorrbolgInventoryManagerComponent>("Corrbolg Inventory component", false);
	
	InteractableComponent = CreateDefaultSubobject<UTartarusInteractableSourceComponent>("InteractableComponent", false);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent", false);
}

void ATartarusPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* const EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ATartarusPlayerController::TryInteract);
	}
}

void ATartarusPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (IsValid(AbilitySystemComponent))
	{
		InteractAbilityHandle = AbilitySystemComponent->GiveAbility(InteractAbility.LoadSynchronous());
		//AbilitySystemComponent->FindAbilitySpecFromHandle(InteractAbilityHandle)->GetAbilityInstances()[0]->GetCurrentActorInfo()->AvatarActor = aPawn;
	}
}

void ATartarusPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->ClearAbility(InteractAbilityHandle);
	}
}

#pragma region GameplayAbility
UAbilitySystemComponent* ATartarusPlayerController::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
#pragma endregion

#pragma region Interaction
void ATartarusPlayerController::TryInteract()
{
	if (!InteractableComponent->TryInteract(GetPawn()->GetTransform()))
	{
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Interaction Failed!")));
#endif
		return;
	}

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Interaction Succes!")));
#endif
}
#pragma endregion

#pragma region UI
void ATartarusPlayerController::ShowPrimaryGameLayoutASync()
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsInitialized())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to show GameLayout: AssetManager is invalid!"), *FString(__FUNCTION__));
		return;
	}

	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &ATartarusPlayerController::HandleGameLayoutLoaded);

	const FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(PrimaryGameLayoutClass.ToSoftObjectPath(), OnRequestCompleted);
	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show GameLayout: Could not start async load!"), *FString(__FUNCTION__));
		return;
	}
}
void ATartarusPlayerController::HandleGameLayoutLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	const TSubclassOf<UTartarusPrimaryGameLayout> PrimaryGameLayoutClassRaw = Cast<UClass>(AssetHandle.Get()->GetLoadedAsset());
	if (!IsValid(PrimaryGameLayoutClassRaw))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show GameLayout: Loaded class was invalid!"), *FString(__FUNCTION__));
		return;
	}

	PrimaryGameLayoutInstance = CreateWidget<UTartarusPrimaryGameLayout>(GetWorld(), PrimaryGameLayoutClassRaw);
	if (!IsValid(PrimaryGameLayoutInstance))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show GameLayout: Could not instantiate the widget!"), *FString(__FUNCTION__));
		return;
	}

	PrimaryGameLayoutInstance->AddToViewport();
}
#pragma endregion
