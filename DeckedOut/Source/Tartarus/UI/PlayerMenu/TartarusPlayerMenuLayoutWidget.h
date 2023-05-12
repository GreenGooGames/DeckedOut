// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"
#include "Engine/DataTable.h"

#include "TartarusPlayerMenuLayoutWidget.generated.h"

class UTartarusSwitcherWidget;
class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusPlayerMenuLayoutWidget : public UTartarusLayoutWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSwitcherWidget> MenuSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher = nullptr;

	virtual void NativeOnInitialized() override;
	void SetupMenuSwitcher();

#pragma region UCommonActivatableWidget
public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion

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
