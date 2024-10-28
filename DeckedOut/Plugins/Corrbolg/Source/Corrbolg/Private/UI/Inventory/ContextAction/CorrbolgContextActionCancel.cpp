// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextAction/CorrbolgContextActionCancel.h"
#include "CommonUserWidget.h"

void UCorrbolgContextActionCancel::Execute(const FCorrbolgContextActionData& Context)
{
	if (!Context.Object.IsValid())
	{
		return;
	}

	UCommonUserWidget* const Widget = Cast<UCommonUserWidget>(Context.Widget.Get());
	Widget->SetVisibility(ESlateVisibility::Collapsed);

}
