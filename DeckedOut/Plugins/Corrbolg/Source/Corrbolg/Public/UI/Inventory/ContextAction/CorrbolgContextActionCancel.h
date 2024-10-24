// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionBase.h"
#include "CorrbolgContextActionCancel.generated.h"

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionCancel : public UCorrbolgContextActionBase
{
	GENERATED_BODY()
	
public:
	virtual void Execute();

};
