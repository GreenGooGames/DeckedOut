// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "TartarusGameMenuWidget.generated.h"

class UTartarusSwitcherWidget;
class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSwitcherWidget> MenuSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher = nullptr;

	virtual void NativeOnInitialized() override;
	void SetupMenuSwitcher();

#pragma region BoundActions
protected:
	// Input action to close the GameMenu.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
		FDataTableRowHandle ExitInputActionData;

private:
	FUIActionBindingHandle ExitActionHandle;

	void RegisterBoundInputActions();
	void HandleExitAction();

#pragma endregion
};