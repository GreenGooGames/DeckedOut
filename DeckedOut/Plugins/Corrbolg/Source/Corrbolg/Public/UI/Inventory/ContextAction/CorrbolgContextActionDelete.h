// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionBase.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionData.h"

#include "CorrbolgContextActionDelete.generated.h"

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionDelete : public UCorrbolgContextActionBase
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FCorrbolgContextActionData& Context) override;
};
