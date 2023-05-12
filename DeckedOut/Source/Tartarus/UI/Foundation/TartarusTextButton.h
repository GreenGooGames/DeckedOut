// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Foundation/TartarusButtonBase.h"
#include "CoreMinimal.h"

#include "TartarusTextButton.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusTextButton : public UTartarusButtonBase
{
	GENERATED_BODY()

public:
	void SetText(const FText& NewText);

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> TextBlock = nullptr;
};
