// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CorrbolgContextActionBase.generated.h"

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Execute() = 0;
};
