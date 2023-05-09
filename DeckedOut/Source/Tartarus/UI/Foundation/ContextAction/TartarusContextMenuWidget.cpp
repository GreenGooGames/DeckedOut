// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"

#include "Components/VerticalBox.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"
#include "UI/Foundation/TartarusTextButton.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "Logging/TartarusLogChannels.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "CommonListView.h"
#include "UI/Foundation/ContextAction/TartarusContextActionListData.h"

void UTartarusContextMenuWidget::SetContext(const UTartarusInventorySlotWidgetData* const Item)
{
	ListView->ClearListItems();

	if (Item->GetItemId() == FTartarusHelpers::InvalidItemId)
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show Context Menu: The Item Id is invalid!"), *FString(__FUNCTION__));
		return;
	}

	// TODO: UI Actions should not be stored inside of the UI agnostic InventoryComponent.
	UTartarusInventoryComponent* const PlayerInventoryComponent = GetOwningPlayer()->FindComponentByClass<UTartarusInventoryComponent>();
	if (!IsValid(PlayerInventoryComponent))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to show Context Menu: Could not find the inventory on the player!"), *FString(__FUNCTION__));
		return;
	}

	const TArray<UTartarusContextAction*>& ContextActions = PlayerInventoryComponent->GetSubInventoryContextActions(Item->GetInventoryStackId().GetInventoryId());
	for (UTartarusContextAction* const ContextAction : ContextActions)
	{
		CreateEntry(ContextAction, Item);
	}
}

bool UTartarusContextMenuWidget::CreateEntry(UTartarusContextAction* const ContextAction, const UTartarusInventorySlotWidgetData* const Item)
{
	if (!IsValid(ListView))
	{
		return false;
	}

	UTartarusContextActionListData* const ListEntry = NewObject<UTartarusContextActionListData>();
	if (!IsValid(ListEntry))
	{
		return false;
	}

	ListEntry->Initialize(ContextAction, this, Item);

	ListView->AddItem(ListEntry);

	return true;
}

#pragma region UCommonActivatableWidget
UWidget* UTartarusContextMenuWidget::NativeGetDesiredFocusTarget() const
{
	if (!IsValid(ListView))
	{
		return nullptr;
	}

	return ListView;
}
#pragma endregion
