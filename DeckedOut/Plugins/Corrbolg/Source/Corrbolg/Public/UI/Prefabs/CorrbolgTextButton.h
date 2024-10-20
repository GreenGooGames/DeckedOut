// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CorrbolgTextButton.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgTextButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	void SetText(const FText& Text) const;

protected:
	/** The text block to show text on the button. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TextBlock = nullptr;

};
