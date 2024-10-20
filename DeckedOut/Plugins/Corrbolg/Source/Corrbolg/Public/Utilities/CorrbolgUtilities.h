// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UCorrbolgInventoryManagerComponent;
class APlayerController;

/**
 * 
 */
struct CORRBOLG_API FCorrbolgUtilities
{
public:
	static UCorrbolgInventoryManagerComponent* FindInventoryComponent(const APlayerController* const PlayerController);
};
