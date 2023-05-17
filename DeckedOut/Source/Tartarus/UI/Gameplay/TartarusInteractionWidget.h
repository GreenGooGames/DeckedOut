// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "TartarusInteractionWidget.generated.h"

class UCommonActionWidget;
class UCommonTextBlock;
class UInputAction;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInteractionWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(const FText& NewText);
	void UpdateInputActionWidget();

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonActionWidget> InputActionWidget;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> TextBlock = nullptr;

	/**
	 *	The enhanced input action that is bound to this button. The common input manager will trigger this button to
	 *	click if the action was pressed
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (EditCondition = "CommonInput.CommonInputSettings.IsEnhancedInputSupportEnabled", EditConditionHides))
		TObjectPtr<UInputAction> TriggeringEnhancedInputAction;
};
