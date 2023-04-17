// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"

#include "TartarusSwitcherWidget.generated.h"

class UCommonTextBlock;
class UCommonVisibilitySwitcher;
class UTartarusTextButton;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusSwitcherWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void LinkVisibilitySwitcher(TWeakObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher);

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusTextButton> PreviousButton = nullptr;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusTextButton> NextButton = nullptr;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> MenuNameText = nullptr;

	virtual void NativeOnInitialized() override;

private:
	TWeakObjectPtr<UCommonVisibilitySwitcher> MenuVisibilitySwitcher = nullptr;

	UFUNCTION()
		void OnPreviousButtonPressed();
	UFUNCTION()
		void OnNextButtonPressed();
	UFUNCTION()
		void OnMenuVisibilityChanged(int32 VisibleWidgetIndex);

};
