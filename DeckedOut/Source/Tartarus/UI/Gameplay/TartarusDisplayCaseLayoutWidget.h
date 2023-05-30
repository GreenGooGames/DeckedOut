// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Foundation/TartarusLayoutWidget.h"

#include "TartarusDisplayCaseLayoutWidget.generated.h"

class UTartarusTransferWidget;
class UTartarusActionBarWidget;
class UTartarusItemDetailsWidget;
class UTartarusInventoryComponent;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusDisplayCaseLayoutWidget : public UTartarusLayoutWidget
{
	GENERATED_BODY()
	
public:
	UTartarusDisplayCaseLayoutWidget();

protected:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTartarusActionBarWidget> ActionBarWidget = nullptr;

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
		FGameplayTag ArtifactsSubInventoryId = FGameplayTag::EmptyTag;

	UTartarusInventoryComponent* FindInteractingInventory() const;
	UTartarusInventoryComponent* FindPlayerInventory() const;
#pragma endregion

};
