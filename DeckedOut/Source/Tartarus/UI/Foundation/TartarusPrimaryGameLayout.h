// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/StreamableManager.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusPrimaryGameLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void PushWidgetToLayerAsync(const FGameplayTag& LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
	void PopWidgetFromLayer(const FGameplayTag& LayerName, UCommonActivatableWidget* const Widget);

protected:
	bool PushWidgetToLayer(const FGameplayTag& LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
		void RegisterLayer(const FGameplayTag& LayerName, UCommonActivatableWidgetContainerBase* const LayerWidget);

private:
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> RegisteredLayers;

#pragma region AsyncLoad
private:
	struct FASyncLoadWidgetClassRequest : public FASyncLoadRequest
	{
		public:
			FGameplayTag LayerName;
	};

	TArray<FASyncLoadWidgetClassRequest> AsyncLoadWidgetClassRequests;

	void HandleASyncLoadWidgetClassCompleted(FGuid RequestId, TSharedPtr<FStreamableHandle> StreamableHandle);
	void OnAsyncLoadWidgetClassCompleted(const FASyncLoadWidgetClassRequest* const Request);
#pragma endregion

#pragma region Input
	void SuspendInput();
	void ResumeInput();

	void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);
#pragma endregion
};
