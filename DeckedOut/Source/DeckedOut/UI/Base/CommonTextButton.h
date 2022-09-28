// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextButton.generated.h"

class UCommonTextBlock;

/**
 * A Common Button Base widget with Text.
 */
UCLASS()
class DECKEDOUT_API UCommonTextButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void SetText(const FText& NewText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TextBlock = nullptr;

};
