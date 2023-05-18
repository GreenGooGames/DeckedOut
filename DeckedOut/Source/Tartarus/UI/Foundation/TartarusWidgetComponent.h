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

	UTartarusWidgetComponent();

	FOnWidgetCreatedEvent& OnWidgetCreatedEvent() { return OnWidgetCreatedEventHandle; }

private:
	FOnWidgetCreatedEvent OnWidgetCreatedEventHandle = FOnWidgetCreatedEvent();

#pragma region UWidgetComponent
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitWidget() override;
#pragma endregion

#pragma region Orientation
protected:
	UPROPERTY(EditDefaultsOnly)
		FVector Offset = FVector::ZeroVector;

	void OrientToUp();
#pragma endregion
};
