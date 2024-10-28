// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CorrbolgContextActionListItem.generated.h"

class UCorrbolgContextActionDefinition;
class UCorrbolgContextActionBase;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgContextActionListItem : public UObject
{
	GENERATED_BODY()

public:
	void SetupContextAction(UCorrbolgContextActionDefinition* const Definition);

	const FText& GetDisplayText() {return DisplayText;}
	UCorrbolgContextActionBase* GetContextAction() const {return ContextAction;}

private:
	UPROPERTY(Transient)
	TObjectPtr<UCorrbolgContextActionBase> ContextAction = nullptr;

	FText DisplayText = FText();
};
