// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CoreMinimal.h"

#include "TartarusContextMenuWidget.generated.h"

class UTartarusContextAction;
class UTartarusInventorySlotWidgetData;
class UTartarusTextButton;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	void SetContextItem(UTartarusInventorySlotWidgetData* const Item);
	TWeakObjectPtr<UTartarusInventorySlotWidgetData> GetContextItem() const { return ContextItem; }

	// Add an entry with behavior.
	bool AddEntry(UTartarusContextAction* const ContextAction);

	bool ContainsEntries() const;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<UTartarusTextButton> EntryTemplate;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UVerticalBox> EntryBox = nullptr;

private:
	// The item that this widget is the context for.
	TWeakObjectPtr<UTartarusInventorySlotWidgetData> ContextItem = nullptr;
};
