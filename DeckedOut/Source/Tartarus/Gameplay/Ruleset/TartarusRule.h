// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "TartarusRule.generated.h"

UCLASS()
class TARTARUS_API UTartarusRule : public UObject
{
	GENERATED_BODY()

public:
	/*
	* Enables the behavior defined by this rule.
	* Return: True if the rule is applied, false if the rule failed to start.
	*/
	virtual bool StartRule(UWorld* const World) { return false; }

	/*
	* Stops the behavior defined by this rule.
	* Return: True if the rule has been reverted, false if the rule is still applied.
	*/
	virtual bool StopRule(UWorld* const World) { return false; }
};
