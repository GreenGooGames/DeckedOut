// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CorrbolgContextActionWidget.generated.h"

class UCorrbolgTextButton;
class UCorrbolgInventorySettings;
class UCorrbolgContextActionBase;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void SetupActions(const UCorrbolgInventorySettings& Settings);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCorrbolgTextButton> ActionButtonClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPanelWidget> ContextActionHolder = nullptr;

private:
	UPROPERTY()
	TArray<TObjectPtr<UCorrbolgContextActionBase>> ActiveContextActions = TArray<TObjectPtr<UCorrbolgContextActionBase>>();

	void Cleanup();
};
