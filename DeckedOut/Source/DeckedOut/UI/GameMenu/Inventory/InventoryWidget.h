// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Engine/DataTable.h"
#include "Input/UIActionBindingHandle.h"

#include "Templates/UnrealTemplate.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/ObjectKey.h"

#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class DECKEDOUT_API UInventoryWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

#pragma region BoundActions
protected:
	// Input action to select the current focused item.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
	FDataTableRowHandle SelectInputActionData;
	
	// Input action to sort the Inventory.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
	FDataTableRowHandle SortInputActionData;

private:
	FUIActionBindingHandle SelectActionHandle;
	FUIActionBindingHandle SortActionHandle;

	void RegisterBoundInputActions();
	void HandleSelectAction();
	void HandleSortAction();

#pragma endregion

};
