// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "TartarusCharacterPreviewWidget.generated.h"

class ASceneCapture2D;
class UCommonLazyImage;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusCharacterPreviewWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<ASceneCapture2D> SceneCapture2DTemplate;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonLazyImage> CharacterPreview = nullptr;

	/*
	* Perform the setup to capture the player character to a rendertarget.
	*/
	void CreateSceneCapture();

	/*
	* Associates the data to be shown in the rendertarget with the scenecapture.
	*/
	void AssociateSceneCapture();

	/*
	* dissociates the data that was shown in the rendertarget with the scenecapture.
	*/
	void DissociateSceneCapture();

private:
	TObjectPtr<ASceneCapture2D> SceneCapture2DInstance = nullptr;
};
