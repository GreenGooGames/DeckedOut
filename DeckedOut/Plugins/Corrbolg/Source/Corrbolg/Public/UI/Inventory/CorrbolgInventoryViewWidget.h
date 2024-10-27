// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTags.h"

#include "CorrbolgInventoryViewWidget.generated.h"

class UCommonTileView;
class UCorrbolgInventoryManagerComponent;
class UCorrbolgInventorySettings;
class UCorrbolgContextActionWidget;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgInventoryViewWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Init(const UCorrbolgInventorySettings& Settings);

	const FText& GetDisplayText() const {return DisplayText;}
	UCommonTileView* const GetTileView() const {return TileView;}

protected:
	/** TileView holding all display entries that represent the entry stored in the inventory. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTileView> TileView = nullptr;

	virtual void NativeOnActivated() override;

	/** Updates the data and refreshes the tileview. */
	void Refresh() const;

private:
	/** The inventory filter to show items for. */
	FGameplayTag Filter = FGameplayTag::EmptyTag;

	/** The text to display when identifying this widget. */
	FText DisplayText = FText();

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCorrbolgContextActionWidget> ContextActionWidget = nullptr;

	void HandleOnItemClicked(UObject* const Item);
};
