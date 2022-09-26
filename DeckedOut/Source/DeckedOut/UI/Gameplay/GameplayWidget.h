// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayWidget.generated.h"

/**
 * Widget containing UI elements visible during gameplay.
 */
UCLASS()
class DECKEDOUT_API UGameplayWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> GameMenuClass = nullptr;

	virtual void NativeOnInitialized() override;

#pragma region BoundActions
protected:
	// Input action to close the GameMenu.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
	FDataTableRowHandle OpenGameMenuInputActionData;

private:
	FUIActionBindingHandle OpenGameMenuActionHandle;

	void RegisterBoundInputActions();
	void HandleOpenGameMenuAction() const;

#pragma endregion
};
