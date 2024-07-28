// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonButtonBase.h"

#include "CorrbolgInventoryViewEntryWidget.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewEntryWidget : public UCommonButtonBase, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> DisplayImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> StackSizeText = nullptr;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

};
