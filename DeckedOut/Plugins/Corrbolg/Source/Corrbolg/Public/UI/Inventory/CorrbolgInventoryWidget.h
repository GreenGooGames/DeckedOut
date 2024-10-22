// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "CorrbolgInventoryWidget.generated.h"

class UCorrbolgNavigationWidget;
class UCommonVisibilitySwitcher;
class UCorrbolgEntryPreview;
class UCorrbolgInventoryManagerComponent;
class UCorrbolgInventoryViewWidget;

/**
 * The main widget that defines the layout of the inventory.
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

#pragma region Navigation bar
protected:
	/* Widget containing the nagivation tools to switch between different inventories. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCorrbolgNavigationWidget> NavigationBar = nullptr;

private:
	/* Links the navigation bar to the Visibility Switcher. */
	void ConstructNavigationBar();
#pragma endregion

#pragma region Inventory Overview
protected:
	/* Visibility Switcher owning all inventory overviews. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> InventorySwitcher = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCorrbolgInventoryViewWidget> InventoryViewClass = nullptr;

private:
	/* Creates the inventory overview widgets for each inventory owned.	*/
	void ConstructInventoryView();

	/** Handles behavior to execute when the selected entry in the Tileview has changed.*/
	UFUNCTION()
	void HandleItemSelectionChanged(UObject* Item);
#pragma endregion

#pragma region Highlighted Entry Preview
protected:
	/* Widget showing a preview of the item highligted in the inventory overview. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCorrbolgEntryPreview> EntryPreview = nullptr;
#pragma endregion
};
