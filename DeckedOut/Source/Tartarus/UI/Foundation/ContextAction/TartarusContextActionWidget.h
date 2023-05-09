// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "CoreMinimal.h"
#include "UI/Foundation/TartarusTextButton.h"

#include "TartarusContextActionWidget.generated.h"

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextActionWidget : public UTartarusTextButton, public IUserObjectListEntry
{
	GENERATED_BODY()
	
#pragma region IUserObjectListEntry
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
#pragma endregion

#pragma region UCommonButton
protected:
	virtual void NativeOnClicked() override;
#pragma endregion
};
