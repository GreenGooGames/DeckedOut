// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DOPlayerController.generated.h"

class UInventoryComponent;
class UInteractionComponent;
class UPrimaryGameLayout;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API ADOPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ADOPlayerController();
	virtual void SetupInputComponent() override;

	void TryInteract();
	void DEBUG_DropItem();

	UPROPERTY(EditDefaultsOnly)
	int32 DEBUG_ItemToDropId = 0;

	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; };
	
	void ShowPrimaryGameLayout();
	TObjectPtr<UPrimaryGameLayout> GetPrimaryGameLayout() const { return PrimaryGameLayoutInstance; };

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UPrimaryGameLayout> PrimaryGameLayoutClass = nullptr;

private:
	TObjectPtr<UPrimaryGameLayout> PrimaryGameLayoutInstance = nullptr;
};
