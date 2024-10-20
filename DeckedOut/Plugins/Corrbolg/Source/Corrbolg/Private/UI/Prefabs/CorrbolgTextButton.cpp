// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Prefabs/CorrbolgTextButton.h"

#include "CommonTextBlock.h"

void UCorrbolgTextButton::SetText(const FText& Text) const
{
	if(IsValid(TextBlock))
	{
		TextBlock->SetText(Text);
	}
}
