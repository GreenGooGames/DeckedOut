// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonButtonBase.h"

#include "CorrbolgContextActionEntryWidget.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionEntryWidget : public UCommonButtonBase, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TextBlock = nullptr;
};
