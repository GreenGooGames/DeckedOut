// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckedOut/UI/GameMenu/Inventory/InventoryWidget.h"

#include "CommonTileView.h"
#include "DeckedOut/Components/Inventory/InventoryComponent.h"
#include "DeckedOut/Characters/Player/DOPlayerController.h"
#include "DeckedOut/UI/GameMenu/Inventory/InventorySlotWidget.h"
#include "Input/CommonUIInputTypes.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
	ConstructInventoryView();
}

void UInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	SetupInventoryView();
}

void UInventoryWidget::ConstructInventoryView()
{
	const ADOPlayerController* const PlayerController = GetOwningPlayer<ADOPlayerController>();

	if (PlayerController)
	{
		const UInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();
		
		if (InventoryComponent)
		{
			const TArray<FItemData_Inventory>& StoredItems = InventoryComponent->GetStoredItems();

			for (const FItemData_Inventory& Item : StoredItems)
			{
				UInventorySlotWidget* const SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), TileView->GetEntryWidgetClass());

				if (SlotWidget)
				{
					SlotWidget->SetupDisplayData(Item);
					TileView->AddItem(SlotWidget);
				}
			}
		}
	}
}

void UInventoryWidget::SetupInventoryView()
{
	const ADOPlayerController* const PlayerController = GetOwningPlayer<ADOPlayerController>();

	if (PlayerController)
	{
		const UInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();

		if (InventoryComponent)
		{
			const TArray<FItemData_Inventory>& StoredItems = InventoryComponent->GetStoredItems();

			for (int32 i = 0; i < StoredItems.Num(); i++)
			{
				UInventorySlotWidget* const SlotWidget = Cast<UInventorySlotWidget>(TileView->GetItemAt(i));

				if (SlotWidget)
				{
					SlotWidget->SetupDisplayData(StoredItems[i]);
				}
			}
		}
	}
}

#pragma region BoundActions
void UInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SelectBoundActionArguments = FBindUIActionArgs(SelectInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectAction));
	SelectBoundActionArguments.bDisplayInActionBar = true;

	SelectActionHandle = RegisterUIActionBinding(SelectBoundActionArguments);

	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UInventoryWidget::HandleSelectAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}

void UInventoryWidget::HandleSortAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}
#pragma endregion