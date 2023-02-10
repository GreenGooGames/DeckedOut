// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusInventoryWidget.generated.h"

class UCommonVisibilitySwitcher;
class UTartarusSubInventoryView;
class UTartarusSwitcherWidget;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventoryWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

protected:
	/*
	* Reference to the class used to create the sub-inventory views.
	*/
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusSubInventoryView> SubInventoryViewClass = nullptr;

	/*
	* Creates a Sub-Inventory view widget for each Sub-Inventory owned by the owner inventory component.
	*/
	void ConstructInventoryView();

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

#pragma region Navbar
protected:
	/*
	* Widget containing the nagivation tools to switch between different sub-menu's.
	*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSwitcherWidget> SubInventoryMenuSwitcher = nullptr;

	/*
	* Visibility Switcher owning all sub-inventories.
	*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonVisibilitySwitcher> SubInventoryVisibilitySwitcher = nullptr;

	/*
	* Links the MenuSwitcher to the Visibility Switcher.
	*/
	void SetupMenuSwitcher();
#pragma endregion
};
