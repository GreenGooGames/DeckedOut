// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"
#include "GameplayTagContainer.h"

#include "TartarusCardReaderLayoutWidget.generated.h"

class UTartarusActionBarWidget;
class UTartarusSubInventoryViewWidget;
class UTartarusItemSelectionWidget;
class UTartarusItemDetailsWidget;
class UTartarusGameModifiersWidget;
class UTartarusContextMenuWidget;
class UTartarusContextActionBulk;
class UTartarusInventorySlotWidgetData;
class UTartarusInventoryComponent;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCardReaderLayoutWidget : public UTartarusLayoutWidget
{
	GENERATED_BODY()

public:
	UTartarusCardReaderLayoutWidget();

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusActionBarWidget> ActionBarWidget = nullptr;
		
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusGameModifiersWidget> GameModifiersWidget = nullptr;

#pragma region UCommonActivatableWidget
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual void NativeOnInitialized() override;
#pragma endregion

#pragma region ItemInfo
protected:
	void InitializeItemInfoWidget();
	void HandleItemSelectionChanged(UObject* Item);

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusItemDetailsWidget> ItemDetailsWidget = nullptr;
#pragma endregion

#pragma region Inventory
protected:
	bool TransferToInventory(const UTartarusInventorySlotWidgetData* const ItemData, UTartarusInventoryComponent* const Host, UTartarusInventoryComponent* const Recipient);

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
		FGameplayTag SubInventoryId = FGameplayTag::EmptyTag;

#pragma region CardReaderInventory
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSubInventoryViewWidget> CardReaderInventoryWidget = nullptr;

	void InitializeCardReaderInventoryWidget();
	void HandleCardReaderInventoryItemClicked(UObject* Item);
#pragma endregion

#pragma region PlayerInventory
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusSubInventoryViewWidget> PlayerInventoryWidget = nullptr;

	void InitializePlayerInventoryWidget();
	void HandlePlayerInventoryItemClicked(UObject* Item);
#pragma endregion
#pragma endregion
};
