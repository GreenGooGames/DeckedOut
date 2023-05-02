// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusInventoryWidget.generated.h"

class UCommonActivatableWidgetStack;
class UCommonVisibilitySwitcher;
class UTartarusContextMenu;
class UTartarusInventoryInfoWidget;
class UTartarusInventorySlotWidgetData;
class UTartarusSubInventoryView;
class UTartarusSwitcherWidget;
class ASceneCapture2D;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusInventoryWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

#pragma region Sub-Inventory
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
#pragma endregion

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

#pragma region ItemInfo
protected:
	UFUNCTION()
		void HandleItemFocusChanged(UObject* Item, bool bIsHovered);

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusInventoryInfoWidget> SelectedItemInfo = nullptr;
#pragma endregion

#pragma region Context
protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusContextMenu> ContextMenuTemplate;

	void HandleItemClicked(UObject* Item);

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonActivatableWidgetStack> ContextStack = nullptr;
#pragma endregion

#pragma region PlayerPreview
protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<ASceneCapture2D> SceneCapture2DTemplate;

	/*
	* Perform the setup to capture the player character to a rendertarget.
	*/
	void CreateSceneCapture();

	/*
	* Associates the data to be shown in the rendertarget with the scenecapture.
	*/
	void AssociateSceneCapture();

	/*
	* dissociates the data that was shown in the rendertarget with the scenecapture.
	*/
	void DissociateSceneCapture();

private:
	TObjectPtr<ASceneCapture2D> SceneCapture2DInstance = nullptr;
#pragma endregion
};
