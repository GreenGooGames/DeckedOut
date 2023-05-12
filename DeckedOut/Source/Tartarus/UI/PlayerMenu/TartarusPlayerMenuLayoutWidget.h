// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"

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
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion
};
