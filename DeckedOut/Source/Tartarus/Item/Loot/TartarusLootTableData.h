// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "TartarusLootTableData.Generated.h"

USTRUCT()
struct FLootTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		FDataTableRowHandle Item = FDataTableRowHandle();
};
