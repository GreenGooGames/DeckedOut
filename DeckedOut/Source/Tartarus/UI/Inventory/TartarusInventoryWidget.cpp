// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/TartarusInventoryWidget.h"

#include "CommonTileView.h"
#include "Input/CommonUIInputTypes.h"
#include "Item/Inventory/TartarusInventoryComponent.h"
#include "Item/System/TartarusItemSubsystem.h"
#include "Item/TartarusItemData.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerController.h"
#include "UI/Inventory/TartarusInventorySlotWidget.h"

void UTartarusInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterBoundInputActions();
	ConstructInventoryView();
}

void UTartarusInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	SetupInventoryView();
}

void UTartarusInventoryWidget::ConstructInventoryView()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No player controller!"), __FUNCTION__);
		return;
	}

	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: No inventory found!"), __FUNCTION__);
		return;
	}

	const TArray<FInventoryItemStack>& StoredItems = InventoryComponent->GetOverview();

	for (const FInventoryItemStack& Item : StoredItems)
	{
		UTartarusInventorySlotWidget* const SlotWidget = CreateWidget<UTartarusInventorySlotWidget>(GetWorld(), TileView->GetEntryWidgetClass());

		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create an instance of Inventory Tile view!"), __FUNCTION__);
			continue;
		}

		AsyncRequestSetDisplayTexture(SlotWidget, Item.ItemId);
		TileView->AddItem(SlotWidget);
	}

}

void UTartarusInventoryWidget::SetupInventoryView()
{
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No player controller!"), __FUNCTION__);
		return;
	}
	
	const UTartarusInventoryComponent* const InventoryComponent = PlayerController->GetInventoryComponent();

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No inventory found!"), __FUNCTION__);
		return;
	}
	
	const TArray<FInventoryItemStack>& StoredItems = InventoryComponent->GetOverview();

	for (int32 i = 0; i < StoredItems.Num(); i++)
	{
		UTartarusInventorySlotWidget* const SlotWidget = Cast<UTartarusInventorySlotWidget>(TileView->GetItemAt(i));

		if (!IsValid(SlotWidget))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Setup inventory view failed: No tile found!"), __FUNCTION__);
			continue;
		}

		AsyncRequestSetDisplayTexture(SlotWidget, StoredItems[i].ItemId);
	}
}

#pragma region BoundActions
void UTartarusInventoryWidget::RegisterBoundInputActions()
{
	FBindUIActionArgs SelectBoundActionArguments = FBindUIActionArgs(SelectInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectAction));
	SelectBoundActionArguments.bDisplayInActionBar = true;

	SelectActionHandle = RegisterUIActionBinding(SelectBoundActionArguments);

	FBindUIActionArgs SortBoundActionArguments = FBindUIActionArgs(SortInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSortAction));
	SortBoundActionArguments.bDisplayInActionBar = true;

	SortActionHandle = RegisterUIActionBinding(SortBoundActionArguments);
}

void UTartarusInventoryWidget::HandleSelectAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}

void UTartarusInventoryWidget::HandleSortAction()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Green, FString(__FUNCTION__));
#endif
}
#pragma endregion

#pragma region ASyncLoading
void UTartarusInventoryWidget::AsyncRequestSetDisplayTexture(UTartarusInventorySlotWidget* const SlotWidget, const int32 ItemId)
{
	UTartarusItemSubsystem* const ItemSubsystem = GetWorld()->GetSubsystem<UTartarusItemSubsystem>();

	if (!IsValid(ItemSubsystem))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Construct inventory view failed: ItemSubsystem was invalid!"), __FUNCTION__);
		return;
	}

	// BEGIN [Koen Goossens] TODO: ASync
	UDataTable* const ItemDataTable = ItemSubsystem->ItemDataTable.LoadSynchronous();

	if (!IsValid(ItemDataTable))
	{
		return;
	}

	FString Context = "";
	TArray<FItemTableRow*> Items;

	ItemDataTable->GetAllRows<FItemTableRow>(Context, Items);

	if (Items.IsEmpty())
	{
		return;
	}

	const FItemTableRow* ItemRow = nullptr;

	// Iterate all rows untill the wanted one is found.
	for (FItemTableRow* const ItemTableRow : Items)
	{
		if (ItemTableRow->UniqueId == ItemId)
		{
			ItemRow = ItemTableRow;
		}
	}

	if (!ItemRow)
	{
		return;
	}

	SlotWidget->SetDisplayTexture(ItemRow->DisplayTexture.LoadSynchronous());
	// END [Koen Goossens] TODO: ASync
}
#pragma endregion
