// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/DataTable.h"
#include "DeckedOut/World/Items/Item.h"
#include "DeckedOut/World/Items/ItemDataAsset.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ItemManagerSubsystem.generated.h"

USTRUCT()
struct FItemsTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UItemDataAsset> ItemDataAsset;
};

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UItemManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetItemsDataTable(const TSoftObjectPtr<UDataTable>& InItemsDataTable);

	UFUNCTION(BlueprintCallable)
	bool SpawnItem(const int32 ItemId, const FTransform& SpawnTransform);
	UFUNCTION(BlueprintCallable)
	bool DespawnItem(const TSoftObjectPtr<AItem>& Item);

protected:
	TObjectPtr<UDataTable> ItemsDataTable;
	TArray<TObjectPtr<AItem>> SpawnedItems;
};
