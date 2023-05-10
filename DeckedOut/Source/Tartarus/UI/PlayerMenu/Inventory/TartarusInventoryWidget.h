// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusInventoryWidget.generated.h"

class UCommonActivatableWidgetStack;
class UCommonVisibilitySwitcher;
class UTartarusContextMenuWidget;
class UTartarusInventoryInfoWidget;
class UTartarusInventorySlotWidgetData;
class UTartarusSubInventoryViewWidget;
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
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

#pragma region Sub-Inventory
protected:
	/*
	* Reference to the class used to create the sub-inventory views.
	*/
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusSubInventoryViewWidget> SubInventoryViewClass = nullptr;

	/*
	* Creates a Sub-Inventory view widget for each Sub-Inventory owned by the owner inventory component.
	*/
	void ConstructInventoryView();
#pragma endregion

#pragma region BoundActions
protected:
	// Input action to sort the Inventory.
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (RowType = CommonInputActionDataBase))
		FDataTableRowHandle SortInputActionData;

private:
	FUIActionBindingHandle SortActionHandle;

	void RegisterBoundInputActions();
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

#pragma region ItemInfo
protected:
	void HandleItemSelectionChanged(UObject* Item);

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusInventoryInfoWidget> SelectedItemInfo = nullptr;
#pragma endregion

#pragma region Context
protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusContextMenuWidget> ContextMenuTemplate;

	void HandleItemClicked(UObject* Item);

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonActivatableWidgetStack> ContextStack = nullptr;
#pragma endregion
};
