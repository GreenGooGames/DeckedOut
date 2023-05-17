// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/TartarusInteractionWidget.h"

#include "CommonTextBlock.h"
#include "CommonActionWidget.h"
#include "InputAction.h"
#include "CommonUITypes.h"

void UTartarusInteractionWidget::SetText(const FText& NewText)
{
	if (TextBlock)
	{
		TextBlock->SetText(NewText);
	}
}

void UTartarusInteractionWidget::UpdateInputActionWidget()
{
	const bool bIsEnhancedInputSupportEnabled = CommonUI::IsEnhancedInputSupportEnabled();
	if (bIsEnhancedInputSupportEnabled)
	{
		InputActionWidget->SetEnhancedInputAction(TriggeringEnhancedInputAction);
	}
}
