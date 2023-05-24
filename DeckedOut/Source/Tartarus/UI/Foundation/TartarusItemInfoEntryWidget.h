// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "TartarusItemInfoEntryWidget.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemInfoEntryWidget : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> MainText = nullptr;
	
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> SubText = nullptr;

	void SetText(const FText& InMainText, const FText& InSubText);

#pragma region IUserObjectListEntry
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
#pragma endregion
};
