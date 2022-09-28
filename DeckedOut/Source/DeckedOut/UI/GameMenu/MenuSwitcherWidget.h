// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuSwitcherWidget.generated.h"

class UCommonTextButton;
class UCommonTextBlock;
class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UMenuSwitcherWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void LinkVisibilitySwitcher(TWeakObjectPtr<UCommonVisibilitySwitcher> VisibilitySwitcher);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextButton> PreviousButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextButton> NextButton = nullptr;
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
