// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusGameModifiersWidget.generated.h"

enum class EGameModifier : uint8;

class UCommonListView;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusGameModifiersWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

	void Refresh();
	void HandleGameModifiersChanged(const EGameModifier Modifier, const float Adjustment);

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonListView> GameModifierList = nullptr;
};
