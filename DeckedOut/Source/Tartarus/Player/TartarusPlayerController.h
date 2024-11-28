// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySpecHandle.h"
#include "TartarusPlayerController.generated.h"

class UAbilitySystemComponent;
class UInputAction;
class UTartarusInventoryComponent;
class UTartarusInteractableSourceComponent;
class UTartarusPrimaryGameLayout;
class UCorrbolgInventoryManagerComponent;
class UGameplayAbility;

struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusPlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATartarusPlayerController();
	virtual void SetupInputComponent() override;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

#pragma region GameplayAbility
public:
	/** Returns the Ability System Component. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	/** Ability System Component. Required to use Gameplay Attributes and Gameplay Abilities. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
#pragma endregion

#pragma region Interaction
public:
	// Tells the InteractableComponent to try interact with an interactable target.
	void TryInteract();

protected:
	// Component responsible for interaction between source and targets.
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInteractableSourceComponent> InteractableComponent = nullptr;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* InteractAction;

	/** Ability that gives the player the ability to interact with objects. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TSoftObjectPtr<UGameplayAbility> InteractAbility = nullptr;

	UPROPERTY(Transient)
	FGameplayAbilitySpecHandle InteractAbilityHandle = FGameplayAbilitySpecHandle();
#pragma endregion

#pragma region Inventory
public:
	FORCEINLINE UTartarusInventoryComponent* GetTartarusInventoryComponent() const { return TartarusInventoryComponent; };
	FORCEINLINE UCorrbolgInventoryManagerComponent* GetInventoryComponent() const { return CorrbolgInventoryComponent; };

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> TartarusInventoryComponent = nullptr;
			
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UCorrbolgInventoryManagerComponent> CorrbolgInventoryComponent = nullptr;

#pragma endregion

#pragma region UI
public:
	void ShowPrimaryGameLayoutASync();
	FORCEINLINE TObjectPtr<UTartarusPrimaryGameLayout> GetPrimaryGameLayout() const { return PrimaryGameLayoutInstance; };

protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusPrimaryGameLayout> PrimaryGameLayoutClass = nullptr;

	void HandleGameLayoutLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

private:
	TObjectPtr<UTartarusPrimaryGameLayout> PrimaryGameLayoutInstance = nullptr;
#pragma endregion
};
