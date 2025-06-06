// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionData.h"

#include "CorrbolgContextActionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CORRBOLG_API UCorrbolgContextActionBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FCorrbolgContextActionData& Context) PURE_VIRTUAL(UCorrbolgContextActionBase::Execute, return;);
};
