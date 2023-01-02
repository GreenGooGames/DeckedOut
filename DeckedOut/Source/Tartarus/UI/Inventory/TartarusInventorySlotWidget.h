// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "TartarusInventorySlotWidget.generated.h"

class UCommonLazyImage;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventorySlotWidget : public UCommonActivatableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	void SetDisplayTexture(UTexture2D* Texture);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> DisplayImage = nullptr;

	UPROPERTY(EditDefaultsOnly)
		FSlateBrush EmptySlotBrush = FSlateBrush();

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
