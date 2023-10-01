// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusPrimaryGameLayout.h"

#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "UI/Foundation/TartarusLayoutWidget.h"
#include "CommonInputSubsystem.h"
#include "TartarusPrimaryGameLayout.h"
#include "Player/TartarusPlayerController.h"

UTartarusPrimaryGameLayout* const UTartarusPrimaryGameLayout::Get(AController* PlayerController)
{
	ATartarusPlayerController* Controller = Cast<ATartarusPlayerController>(PlayerController);
	if (!IsValid(Controller))
	{
		return nullptr;
	}

	return 	Controller->GetPrimaryGameLayout();
}

void UTartarusPrimaryGameLayout::PushWidgetToLayerAsync(const FGameplayTag& LayerName, TSoftClassPtr<UTartarusLayoutWidget> WidgetClass)
{
	// Validate Input.
	if (!LayerName.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: LayerName is invalid!"), *FString(__FUNCTION__));
		return;
	}

	if (!RegisteredLayers.Contains(LayerName))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: No Layer registered for given GameplayTag!"), *FString(__FUNCTION__));
		return;
	}

	// Retrieve the AssetManager that handles async loading.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();
	if (!AssetManager.IsInitialized())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: Could not retrieve the AssetManager!"), *FString(__FUNCTION__));
		return;
	}

	// Prepare a callback for when the loading finishes.
	FAsyncLoadAssetRequestCompletedEvent OnLoadCompleted = FAsyncLoadAssetRequestCompletedEvent();
	OnLoadCompleted.AddUObject(this, &UTartarusPrimaryGameLayout::HandleASyncLoadWidgetClassCompleted);

	// Start the AsyncLoad of the Widget Class.
	const FGuid AsyncLoadId = AssetManager.AsyncRequestLoadAsset(WidgetClass.ToSoftObjectPath(), OnLoadCompleted);
	if (!AsyncLoadId.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: Could not retrieve the AssetManager!"), *FString(__FUNCTION__));
		return;
	}

	FASyncLoadWidgetClassRequest ASyncLoadRequest = FASyncLoadWidgetClassRequest();
	ASyncLoadRequest.SetASyncLoadRequestId(AsyncLoadId);
	ASyncLoadRequest.LayerName = LayerName;

	AsyncLoadWidgetClassRequests.Add(ASyncLoadRequest);
}

bool UTartarusPrimaryGameLayout::PushWidgetToLayer(const FGameplayTag& LayerName, TSubclassOf<UTartarusLayoutWidget> WidgetClass)
{
	if (!IsValid(WidgetClass))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: The class was invalid!"), *FString(__FUNCTION__));
		return false;
	}

	UTartarusLayoutWidget* const NewWidget = RegisteredLayers[LayerName]->AddWidget<UTartarusLayoutWidget>(WidgetClass);
	if (!IsValid(NewWidget))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: Failed to Add the Widget!"), *FString(__FUNCTION__));
		return false;
	}

	NewWidget->SetOwningLayerName(LayerName);

	return true;
}

void UTartarusPrimaryGameLayout::PopWidgetFromLayer(const FGameplayTag& LayerName, UTartarusLayoutWidget* const Widget)
{
	// Validate Input.
	if (!LayerName.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To pop Widget from layer: LayerName is invalid!"), *FString(__FUNCTION__));
		return;
	}

	if (!RegisteredLayers.Contains(LayerName))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To pop Widget from Layer: No Layer registered for given GameplayTag!"), *FString(__FUNCTION__));
		return;
	}

	if (!IsValid(Widget))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To pop Widget from Layer: Widget is not valid!"), *FString(__FUNCTION__));
		return;
	}

	RegisteredLayers[LayerName]->RemoveWidget(*Widget);
}

void UTartarusPrimaryGameLayout::RegisterLayer(const FGameplayTag& LayerName, UCommonActivatableWidgetContainerBase* const LayerWidget)
{
	if (!LayerName.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To register Layer: LayerName is invalid!"), *FString(__FUNCTION__));
		return;
	}

	if (!IsValid(LayerWidget))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To register Layer: WidgetStack is invalid!"), *FString(__FUNCTION__));
		return;
	}

	if (RegisteredLayers.Contains(LayerName))
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To register Layer: Entry already exists!"), *FString(__FUNCTION__));
		return;
	}

	LayerWidget->OnTransitioningChanged.AddUObject(this, &ThisClass::OnWidgetStackTransitioning);

	RegisteredLayers.Add(LayerName, LayerWidget);
}

#pragma region AsyncLoad
void UTartarusPrimaryGameLayout::HandleASyncLoadWidgetClassCompleted(FGuid RequestId, TSharedPtr<FStreamableHandle> StreamableHandle)
{
	// Get the request that is being handled.
	FASyncLoadWidgetClassRequest* const CurrentRequest = AsyncLoadWidgetClassRequests.FindByPredicate([&RequestId](const FASyncLoadWidgetClassRequest& Request)
		{
			return Request.GetASyncLoadRequestId() == RequestId;
		});

	if (CurrentRequest == nullptr)
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: Request not found after loading the Widget Class!"), *FString(__FUNCTION__));
		return OnAsyncLoadWidgetClassCompleted(CurrentRequest);
	}

	// Validate the loaded Asset.
	if (!StreamableHandle.IsValid())
	{
		UE_LOG(LogTartarus, Error, TEXT("%s: Unable To push Widget to Layer: The streamable handle is invalid!"), *FString(__FUNCTION__));
		return OnAsyncLoadWidgetClassCompleted(CurrentRequest);
	}

	TSubclassOf<UTartarusLayoutWidget> WidgetClass = Cast<UClass>(StreamableHandle.Get()->GetLoadedAsset());
	PushWidgetToLayer(CurrentRequest->LayerName, WidgetClass);

	OnAsyncLoadWidgetClassCompleted(CurrentRequest);
}

void UTartarusPrimaryGameLayout::OnAsyncLoadWidgetClassCompleted(const FASyncLoadWidgetClassRequest* const Request)
{
	if (Request == nullptr)
	{
		return;
	}

	AsyncLoadWidgetClassRequests.RemoveSingleSwap(*Request);
}
#pragma endregion

#pragma region Input
void UTartarusPrimaryGameLayout::SuspendInput()
{
	UCommonInputSubsystem* const CommonInputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer());
	if (!IsValid(CommonInputSubsystem))
	{
		return;
	}

	FName SuspendToken = "UITransition";

	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);

	GetOwningPlayer()->FlushPressedKeys();
}

void UTartarusPrimaryGameLayout::ResumeInput()
{
	UCommonInputSubsystem* const CommonInputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer());
	if (!IsValid(CommonInputSubsystem))
	{
		return;
	}
	
	FName SuspendToken = "UITransition";

	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
	CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
}

void UTartarusPrimaryGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		SuspendInput();
	}
	else
	{
		ResumeInput();
	}
}
#pragma endregion
