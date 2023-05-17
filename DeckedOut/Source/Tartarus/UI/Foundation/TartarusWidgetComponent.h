// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "TartarusWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	DECLARE_EVENT_OneParam(UTartarusWidgetComponent, FOnWidgetCreatedEvent, UTartarusWidgetComponent*);

	FOnWidgetCreatedEvent& OnWidgetCreatedEvent() { return OnWidgetCreatedEventHandle; }

	virtual void InitWidget() override;

private:
	FOnWidgetCreatedEvent OnWidgetCreatedEventHandle = FOnWidgetCreatedEvent();
};
