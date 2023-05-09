// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"

#include "TartarusContextMenuWidget.generated.h"

class UTartarusContextAction;
class UTartarusInventorySlotWidgetData;
class UTartarusTextButton;
class UCommonListView;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void SetContext(const UTartarusInventorySlotWidgetData* const SlotWidgetData);

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonListView> ListView = nullptr;

	bool CreateEntry(UTartarusContextAction* const ContextAction, const UTartarusInventorySlotWidgetData* const Item);

#pragma region UCommonActivatableWidget
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion
};
