// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionBase.h"

#include "CorrbolgContextActionDefinition.generated.h"

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const TSoftClassPtr<UCorrbolgContextActionBase>& GetContextActionClass() const {return ContextActionClass;}
	const FText& GetActionName() {return ActionName;}

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FText ActionName = FText();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText ActionDescription = FText();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UCorrbolgContextActionBase> ContextActionClass = nullptr;
};
