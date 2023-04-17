// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"

#include "TartarusGameplayWidget.generated.h"

/**
 * Widget containing UI elements visible during gameplay.
 */
UCLASS()
class TARTARUS_API UTartarusGameplayWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	// Reference to the class containing the game menu to display.
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
