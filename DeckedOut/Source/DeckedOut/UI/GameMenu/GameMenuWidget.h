// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameMenuWidget.generated.h"

class UMenuSwitcherWidget;
class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UGameMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuSwitcherWidget> MenuSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher = nullptr;

	virtual void NativeOnInitialized() override;
	void SetupMenuSwitcher();

#pragma region BoundActions
protected:
	// Input action to close the GameMenu.
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle ExitInputActionData;

private:
	FUIActionBindingHandle ExitActionHandle;

	void RegisterBoundInputActions();
	void HandleExitAction();

#pragma endregion
};
