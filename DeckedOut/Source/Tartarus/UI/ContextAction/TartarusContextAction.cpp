// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContextAction/TartarusContextAction.h"

#include "UI/ContextAction/TartarusContextMenuWidget.h"

void UTartarusContextAction::SetParentMenu(UTartarusContextMenuWidget* const Menu)
{
	ParentMenu = Menu;
}
