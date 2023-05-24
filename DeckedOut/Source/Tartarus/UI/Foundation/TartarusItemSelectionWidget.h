// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"

#include "TartarusItemSelectionWidget.generated.h"

class UCommonTileView;
class UTartarusInventorySlotWidgetData;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusItemSelectionWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	FORCEINLINE UCommonTileView* GetTileView() { return TileView; }

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonTileView> TileView = nullptr;

	UPROPERTY(EditInstanceOnly)
		int NumEntries = 0;

	void Populate();
	UTartarusInventorySlotWidgetData* CreateListItemData();

#pragma region UCommonActivatableWidget
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion
};
