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
		TObjectPtr<UTartarusSubInventoryViewWidget> InventoryWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusItemSelectionWidget> ActiveCardsWidget = nullptr;
	
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusItemDetailsWidget> ItemDetailsWidget = nullptr;
	
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusGameModifiersWidget> GameModifiersWidget = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess=true))
		FGameplayTag SubInventoryId = FGameplayTag::EmptyTag;

#pragma region UCommonActivatableWidget
protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual void NativeOnActivated() override;
#pragma endregion
};
