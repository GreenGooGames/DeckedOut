// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusWidgetComponent.h"

void UTartarusWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (IsValid(GetWidget()))
	{
		OnWidgetCreatedEvent().Broadcast(this);
	}
}
