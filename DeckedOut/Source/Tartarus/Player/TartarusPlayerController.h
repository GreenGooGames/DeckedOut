// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TartarusPlayerController.generated.h"

class UInputAction;
class UTartarusInventoryComponent;
class UTartarusInteractableSourceComponent;
class UTartarusPrimaryGameLayout;

struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class TARTARUS_API ATartarusPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATartarusPlayerController();
	virtual void SetupInputComponent() override;

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
#pragma endregion

#pragma region Inventory
public:
	FORCEINLINE UTartarusInventoryComponent* GetInventoryComponent() const { return InventoryComponent; };

protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UTartarusInventoryComponent> InventoryComponent = nullptr;

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
