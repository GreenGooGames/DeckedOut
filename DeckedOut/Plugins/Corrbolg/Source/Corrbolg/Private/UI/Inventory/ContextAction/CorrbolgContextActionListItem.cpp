// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionListItem.h"
#include "UI/Inventory/ContextAction/CorrbolgContextActionDefinition.h"

void UCorrbolgContextActionListItem::SetupContextAction(UCorrbolgContextActionDefinition* const Definition)
{
	ContextAction = NewObject<UCorrbolgContextActionBase>(this, Definition->GetContextActionClass().LoadSynchronous());

	DisplayText = Definition->GetActionName();
}
