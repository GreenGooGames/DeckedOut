// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"

#include "TartarusInventorySlotWidget.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventorySlotWidget : public UCommonActivatableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> DisplayImage = nullptr;

	UPROPERTY(EditDefaultsOnly)
		FSlateBrush EmptySlotBrush = FSlateBrush();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCommonTextBlock> StackSizeText = nullptr;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void Refresh(const UObject* const ListItemObject);
	void CleanAndListenToUpdateEvent(UObject* const PreviousListItemObject, UObject* const ListItemObject);

private:
	FDelegateHandle OnListItemDataChangedHandle = FDelegateHandle();
};
