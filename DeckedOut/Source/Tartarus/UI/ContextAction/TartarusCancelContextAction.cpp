// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContextAction/TartarusCancelContextAction.h"

#include "UI/Foundation/TartarusContextMenu.h"

void UTartarusCancelContextAction::ExecuteAction()
{
	ParentMenu->DeactivateWidget();
}
