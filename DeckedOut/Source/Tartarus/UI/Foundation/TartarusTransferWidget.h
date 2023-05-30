// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "TartarusTransferWidget.generated.h"

class UTartarusSubInventoryViewWidget;
class UTartarusInventorySlotWidgetData;
class UTartarusInventoryComponent;

DECLARE_EVENT_OneParam(UTartarusTransferWidget, FEntrySelectionChangedEvent, UObject* /*EntryItem*/)

/**
 * Widget to transfer items between inventories.
 */
UCLASS()
class TARTARUS_API UTartarusTransferWidget : public UTartarusActivatableWidget
{
	GENERATED_BODY()

public:
	void LinkInventories(UTartarusInventoryComponent* const HostInventory, UTartarusInventoryComponent* const ClientInventory);
	FEntrySelectionChangedEvent& OnEntrySelectionChanged() { return OnEntrySelectionChangedHandle; }

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSubInventoryViewWidget> HostInventoryWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSubInventoryViewWidget> ClientInventoryWidget = nullptr;

	bool TransferToInventory(const UTartarusInventorySlotWidgetData* const InventorySlotWidgetData, UTartarusInventoryComponent* const SourceInventory, UTartarusInventoryComponent* const TargetInventory);

	void HandleHostInventoryItemClicked(UObject* Item);
	void HandleClientInventoryItemClicked(UObject* Item);

private:
	FEntrySelectionChangedEvent OnEntrySelectionChangedHandle = FEntrySelectionChangedEvent();

#pragma region UCommonActivatableWidget
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion
};
