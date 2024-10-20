// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "CorrbolgNavigationWidget.generated.h"

class UCommonVisibilitySwitcher;
class UCorrbolgTextButton;
class UCommonTextBlock;
class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgNavigationWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupSwitcher(UCommonVisibilitySwitcher* const VisibilitySwitcher);

protected:
	/** The button to navigate to the previous entry. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCorrbolgTextButton> PreviousEntryButton = nullptr;
	
	/** The button to navigate to the next entry. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCorrbolgTextButton> NextEntryButton = nullptr;
	
	/** The text block to display the current navigated entry name. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EntryTitleText = nullptr;

private:
	/** The switcher widget to navigate trough. */
	TWeakObjectPtr<UCommonVisibilitySwitcher> EntryVisibilitySwitcher = nullptr;

	UFUNCTION()
	void OnPreviousButtonPressed();
	
	UFUNCTION()
	void OnNextButtonPressed();
	
	UFUNCTION()
	void OnEntryVisibilityChanged(int32 VisibleWidgetIndex);

};
