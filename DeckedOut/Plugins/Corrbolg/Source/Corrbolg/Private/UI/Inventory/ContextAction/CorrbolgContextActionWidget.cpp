// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionWidget.h"

#include "Inventory/CorrbolgInventorySettings.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionDefinition.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionListItem.h"
#include "CommonListView.h"

void UCorrbolgContextActionWidget::SetupActions(const UCorrbolgInventorySettings& Settings)
{
	for (const TSoftObjectPtr<UCorrbolgContextActionDefinition>& ContextActionDefinition : Settings.GetContextActions())
	{
		UCorrbolgContextActionListItem* const ListItem = NewObject<UCorrbolgContextActionListItem>(this);
		ListItem->SetupContextAction(ContextActionDefinition.LoadSynchronous());

		ContextActionHolder->AddItem(ListItem);
	}
}

