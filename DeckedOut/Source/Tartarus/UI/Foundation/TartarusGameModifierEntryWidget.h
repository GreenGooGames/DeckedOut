// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TartarusGameModifierEntryWidget.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameModifierEntryWidget : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> ModifierName = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> ModifierValue = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> ToolTipTrigger = nullptr;
};
