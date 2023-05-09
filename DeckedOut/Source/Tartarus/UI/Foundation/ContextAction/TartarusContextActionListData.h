// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TartarusContextActionListData.generated.h"

class UTartarusContextAction;
class UTartarusInventorySlotWidgetData;
class UCommonActivatableWidget;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextActionListData : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(UTartarusContextAction* const ContextAction, UCommonActivatableWidget* const ContextMenu, const UTartarusInventorySlotWidgetData* const ContextItem);

	FORCEINLINE const TWeakObjectPtr<UTartarusContextAction>& GetContextAction() const;
	FORCEINLINE const TWeakObjectPtr<const UTartarusInventorySlotWidgetData>& GetContextItemData() const;
	FORCEINLINE const TWeakObjectPtr<UCommonActivatableWidget>& GetContextMenu() const;

private:
	TWeakObjectPtr<UTartarusContextAction> ContextActionHandle;
	TWeakObjectPtr<const UTartarusInventorySlotWidgetData> ContextItemData = nullptr;
	TWeakObjectPtr<UCommonActivatableWidget> ContextMenuHandle;
};
