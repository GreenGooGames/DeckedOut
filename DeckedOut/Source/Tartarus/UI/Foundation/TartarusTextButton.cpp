// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusTextButton.h"

#include "CommonTextBlock.h"

void UTartarusTextButton::SetText(const FText& NewText)
{
	if (TextBlock)
	{
		TextBlock->SetText(NewText);
	}
}
