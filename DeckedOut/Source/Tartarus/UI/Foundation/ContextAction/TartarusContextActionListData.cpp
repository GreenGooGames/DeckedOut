// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusContextActionListData.h"

#include "UI/Foundation/ContextAction/TartarusContextAction.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "CommonActivatableWidget.h"

void UTartarusContextActionListData::Initialize(
	UTartarusContextAction* const ContextAction,
	UCommonActivatableWidget* const ContextMenu,
	const UTartarusInventorySlotWidgetData* const ContextItem)
{
	ContextActionHandle = ContextAction;
	ContextItemData = ContextItem;
	ContextMenuHandle = ContextMenu;
}

const TWeakObjectPtr<UTartarusContextAction>& UTartarusContextActionListData::GetContextAction() const
{
	return ContextActionHandle;
}

const TWeakObjectPtr<const UTartarusInventorySlotWidgetData>& UTartarusContextActionListData::GetContextItemData() const
{
	return ContextItemData;
}

const TWeakObjectPtr<UCommonActivatableWidget>& UTartarusContextActionListData::GetContextMenu() const
{
	return ContextMenuHandle;
}
