// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "CorrbolgNavigationWidget.generated.h"

class UCommonVisibilitySwitcher;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgNavigationWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupSwitcher(const UCommonVisibilitySwitcher* const VisibilitySwitcher);
};
