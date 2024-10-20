// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"

#include "TartarusGameplayHUDLayoutWidget.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameplayHUDLayoutWidget : public UTartarusLayoutWidget
{
	GENERATED_BODY()

protected:
	// Class of widget to show when the PlayerMenu needs to be shwon.
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusLayoutWidget> PlayerMenuClass = nullptr;

	// Layer name of where to add the PlayerMenuClass.
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag PlayerMenuLayer = FGameplayTag();

	virtual void NativeOnInitialized() override;

#pragma region UCommonActivatableWidget
public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
#pragma endregion

#pragma region BoundActions
protected:
	// Input action to open the PlayerMenu.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (EditCondition = "CommonInput.CommonInputSettings.IsEnhancedInputSupportEnabled", EditConditionHides))
		TObjectPtr<UInputAction> OpenPlayerMenuEnhancedInputAction;

private:
	FUIActionBindingHandle OpenPlayerMenuActionHandle;

	void RegisterBoundInputActions();
	void HandleOpenPlayerMenuAction() const;
#pragma endregion
};
