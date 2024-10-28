// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CorrbolgContextActionWidget.generated.h"

class UCorrbolgTextButton;
class UCorrbolgInventorySettings;
class UCorrbolgContextActionBase;
class UCommonListView;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetupActions(const UCorrbolgInventorySettings& Settings);
	
protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonListView> ContextActionHolder = nullptr;

private:
	void HandleOnItemClicked(UObject* const Item);
};
