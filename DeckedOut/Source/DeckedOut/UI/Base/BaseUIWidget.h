// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BaseUIWidget.generated.h"

class UCommonActivatableWidgetStack;

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UBaseUIWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	// Constructs, pushes and activates a new widget to the mainstack.
	void PushWidgetToMainStack(const TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
	// Removes a widget from the mainstack and deactivate it.
	void PopWidgetFromMainStack(UCommonActivatableWidget& WidgetInstance);

private:
	// The mainstack that widgets will be added to.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MainStack = nullptr;

	void OnWidgetPushedToMainStack(const UCommonActivatableWidget* const Widget);
	void OnWidgetPoppedFromMainStack();
};
