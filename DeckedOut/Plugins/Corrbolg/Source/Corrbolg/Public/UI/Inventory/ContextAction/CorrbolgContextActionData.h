// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CorrbolgContextActionData.generated.h"

/**
 * 
 */
 USTRUCT()
struct CORRBOLG_API FCorrbolgContextActionData
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient)
	TWeakObjectPtr<UObject> Object = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<UObject> Widget = nullptr;
};
