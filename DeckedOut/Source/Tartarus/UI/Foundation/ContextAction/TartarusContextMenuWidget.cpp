// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"

#include "Components/VerticalBox.h"
#include "UI/Foundation/ContextAction/TartarusContextAction.h"
#include "UI/Foundation/TartarusTextButton.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"
#include "UI/PlayerMenu/Inventory/TartarusInventorySlotWidgetData.h"

void UTartarusContextMenuWidget::SetContextItem(UTartarusInventorySlotWidgetData* const Item)
{
	ContextItem = Item;
}

bool UTartarusContextMenuWidget::AddEntry(UTartarusContextAction* const ContextAction)
{
	// TODO: Async load the EntryTemplate
	UTartarusTextButton* const Entry = CreateWidget<UTartarusTextButton>(this, EntryTemplate.LoadSynchronous());
	if (!IsValid(Entry))
	{
		return false;
	}

	Entry->SetText(ContextAction->GetName());

	Entry->OnClicked().AddUObject(ContextAction, &UTartarusContextAction::ExecuteAction);
	if (!Entry->OnClicked().IsBound())
	{
		return false;
	}

	EntryBox->AddChild(Entry);

	return true;
}

bool UTartarusContextMenuWidget::ContainsEntries() const
{
	return EntryBox->HasAnyChildren();
}
