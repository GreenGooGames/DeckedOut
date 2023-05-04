// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusCancelContextAction.h"

#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"

void UTartarusCancelContextAction::ExecuteAction()
{
	ParentMenu->DeactivateWidget();
}
