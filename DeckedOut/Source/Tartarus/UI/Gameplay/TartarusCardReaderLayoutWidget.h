// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"

#include "TartarusCardReaderLayoutWidget.generated.h"

class UTartarusActionBarWidget;
class UTartarusGameModifiersWidget;
class UTartarusTransferWidget;
class UTartarusItemDetailsWidget;
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

#pragma region Transfer
protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusTransferWidget> TransferWidget = nullptr;

	void InitializeTransferWidget();

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
		FGameplayTag CardsSubInventoryId = FGameplayTag::EmptyTag;

	UTartarusInventoryComponent* FindCardReaderInventory() const;
	UTartarusInventoryComponent* FindPlayerInventory() const;
#pragma endregion
};
