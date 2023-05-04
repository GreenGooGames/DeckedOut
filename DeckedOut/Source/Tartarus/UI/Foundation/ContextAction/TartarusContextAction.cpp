// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ContextAction/TartarusContextAction.h"

#include "UI/Foundation/ContextAction/TartarusContextMenuWidget.h"

void UTartarusContextAction::SetParentMenu(UTartarusContextMenuWidget* const Menu)
{
	ParentMenu = Menu;
}
