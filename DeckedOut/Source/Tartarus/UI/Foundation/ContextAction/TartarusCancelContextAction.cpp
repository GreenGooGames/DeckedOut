// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusCancelContextAction.h"

#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"
#include "UI/Foundation/ContextAction/TartarusContextActionListData.h"

void UTartarusCancelContextAction::ExecuteAction(UTartarusContextActionListData* const ContextData)
{
	ContextData->GetContextMenu()->DeactivateWidget();
}
