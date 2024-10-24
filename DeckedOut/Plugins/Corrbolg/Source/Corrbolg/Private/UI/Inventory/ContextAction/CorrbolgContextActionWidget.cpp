// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionWidget.h"

#include "Inventory/CorrbolgInventorySettings.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionDefinition.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionBase.h"
#include "UI/Prefabs/CorrbolgTextButton.h"

void UCorrbolgContextActionWidget::SetupActions(const UCorrbolgInventorySettings& Settings)
{
	Cleanup();

	for (const TSoftObjectPtr<UCorrbolgContextActionDefinition>& ContextActionDefinition : Settings.GetContextActions())
	{
		// Create a button for the context action.
		UCorrbolgTextButton* const NewActionButton = NewObject<UCorrbolgTextButton>(this, ActionButtonClass.LoadSynchronous());
		if (!IsValid(NewActionButton))
		{
			return;
		}

		NewActionButton->SetText(ContextActionDefinition.LoadSynchronous()->GetActionName());

		ContextActionHolder->AddChild(NewActionButton);

		// When the button is clicked, execute the contect action behavior.
		UCorrbolgContextActionBase* const ContextAction = NewObject<UCorrbolgContextActionBase>(this, ContextActionDefinition.LoadSynchronous()->GetContextActionClass().LoadSynchronous());
		ActiveContextActions.Add(ContextAction);

		NewActionButton->OnClicked().AddUObject(ContextAction, &UCorrbolgContextActionBase::Execute);
	}
}

void UCorrbolgContextActionWidget::Cleanup()
{
	ContextActionHolder->ClearChildren();

	ActiveContextActions.Empty();
}
