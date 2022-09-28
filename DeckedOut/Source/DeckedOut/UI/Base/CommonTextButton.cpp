// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/Base/CommonTextButton.h"

#include "CommonTextBlock.h"

void UCommonTextButton::SetText(const FText& NewText)
{
	if (TextBlock)
	{
		TextBlock->SetText(NewText);
	}
}