// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusContextActionWidget.h"

#include "UI/Foundation/ContextAction/TartarusContextActionListData.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#pragma region IUserObjectListEntry
void UTartarusContextActionWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UTartarusContextActionListData* const ListItem = Cast<UTartarusContextActionListData>(ListItemObject);
	if (!IsValid(ListItem))
	{
		return;
	}

	SetText(ListItem->GetContextAction()->GetName());
}
#pragma endregion

#pragma region UCommonButton
void UTartarusContextActionWidget::NativeOnClicked()
{
	// Execute the action that is stored in the Data Object.
	UTartarusContextActionListData* const ListItem = GetListItem<UTartarusContextActionListData>();
	if (!IsValid(ListItem))
	{
		return;
	}

	ListItem->GetContextAction()->ExecuteAction(ListItem);
}
#pragma endregion
