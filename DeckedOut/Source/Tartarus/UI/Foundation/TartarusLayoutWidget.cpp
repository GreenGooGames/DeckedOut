// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusLayoutWidget.h"

#include "Player/TartarusPlayerController.h"
#include "UI/Foundation/TartarusPrimaryGameLayout.h"

bool UTartarusLayoutWidget::NativeOnHandleBackAction()
{
	ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		return false;
	}

	UTartarusPrimaryGameLayout* const PrimaryGameLayoutInstance = PlayerController->GetPrimaryGameLayout();
	if (!IsValid(PrimaryGameLayoutInstance))
	{
		return false;
	}

	Super::NativeOnHandleBackAction();
	PrimaryGameLayoutInstance->PopWidgetFromLayer(GetOwningLayerName(), this);

    return true;
}

TOptional<FUIInputConfig> UTartarusLayoutWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void UTartarusLayoutWidget::SetOwningLayerName(const FGameplayTag& LayerName)
{
    OwningLayerName = LayerName;
}

FGameplayTag& UTartarusLayoutWidget::GetOwningLayerName()
{
    return OwningLayerName;
}
