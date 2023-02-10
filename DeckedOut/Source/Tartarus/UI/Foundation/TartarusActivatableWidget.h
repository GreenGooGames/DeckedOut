// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TartarusActivatableWidget.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	const FText& GetLocalizedWidgetName() const { return LocalizedWidgetName; }
	void SetLocalizedWidgetName(const FText& LocalizedName) { LocalizedWidgetName = LocalizedName; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FText LocalizedWidgetName = FText();
};
