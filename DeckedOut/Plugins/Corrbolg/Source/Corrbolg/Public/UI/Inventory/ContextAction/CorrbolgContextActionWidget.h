// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CorrbolgContextActionWidget.generated.h"

class UCorrbolgTextButton;
class UCorrbolgInventorySettings;
class UCorrbolgContextActionBase;
class UCommonListView;

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
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonListView> ContextActionHolder = nullptr;
};
