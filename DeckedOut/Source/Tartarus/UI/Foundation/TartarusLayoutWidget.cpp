// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusLayoutWidget.h"

void UTartarusLayoutWidget::SetOwningLayerName(const FGameplayTag& LayerName)
{
    OwningLayerName = LayerName;
}

FGameplayTag& UTartarusLayoutWidget::GetOwningLayerName()
{
    return OwningLayerName;
}
