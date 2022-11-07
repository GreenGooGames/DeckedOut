// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusPrimaryGameLayout.h"

#include "CommonActivatableWidget.h"
#include "Logging/TartarusLogChannels.h"
#include "System/TartarusAssetManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UTartarusPrimaryGameLayout::PopWidgetFromMainStack(UCommonActivatableWidget& WidgetInstance)
{
	if (MainStack)
	{
		MainStack->RemoveWidget(WidgetInstance);

		OnWidgetPoppedFromMainStack();
	}
}

void UTartarusPrimaryGameLayout::OnWidgetPushedToMainStack(const UCommonActivatableWidget* const Widget)
{
	APlayerController* const PlayerController = GetOwningPlayer();

	if (Widget && PlayerController)
	{
		FInputModeUIOnly InputModeUIOnlyParameters = FInputModeUIOnly();
		UWidget* const DesiredFocusTarget = Widget->GetDesiredFocusTarget();

		if (DesiredFocusTarget)
		{
			InputModeUIOnlyParameters.SetWidgetToFocus(DesiredFocusTarget->TakeWidget());
		}

		PlayerController->SetInputMode(InputModeUIOnlyParameters);
		PlayerController->SetShowMouseCursor(true);
	}
}

void UTartarusPrimaryGameLayout::OnWidgetPoppedFromMainStack()
{
	// If the Mainstack doesn't contain any other widgets (other than the gameplayHUD, reveal the Base UI again.
	if (MainStack->GetNumWidgets() <= 1)
	{
		APlayerController* const PlayerController = GetOwningPlayer();

		if (PlayerController)
		{
			FInputModeGameOnly InputModeGameOnlyParameters = FInputModeGameOnly();
			PlayerController->SetInputMode(InputModeGameOnlyParameters);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

#pragma region AsyncLoading
FGuid UTartarusPrimaryGameLayout::AsyncRequestPushToStack(const TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FPushedToStackEvent& OnRequestCompletedEvent)
{
	// Get the AsyncLoader.
	UTartarusAssetManager& AssetManager = UTartarusAssetManager::Get();

	if (!AssetManager.IsValid())
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to create request: Asset Manager was invalid!"), __FUNCTION__);
		return FGuid();
	}

	// Prepare the callback for when the request completes.
	FAsyncLoadAssetRequestCompletedEvent OnRequestCompleted;
	OnRequestCompleted.AddUObject(this, &UTartarusPrimaryGameLayout::HandleWidgetClassLoaded);

	FGuid AsyncLoadRequestId = AssetManager.AsyncRequestLoadAsset(WidgetClass.ToSoftObjectPath(), OnRequestCompleted);

	if (!AsyncLoadRequestId.IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Failed to create request: Could not start async load!"), __FUNCTION__);
		return FGuid();
	}

	// Setup the request for this spawn.
	FPushToStackRequestInfo PushToStackRequest = FPushToStackRequestInfo(OnRequestCompletedEvent);
	PushToStackRequest.SetASyncLoadRequestId(AsyncLoadRequestId);

	PushToStackRequests.Add(PushToStackRequest);

	return PushToStackRequest.GetRequestId();
}

void UTartarusPrimaryGameLayout::HandleRequestSuccess(const FPushToStackRequestInfo* const SuccessRequest, TWeakObjectPtr<UCommonActivatableWidget> PushedWidget)
{
	if (!SuccessRequest)
	{
		return;
	}

	OnWidgetPushedToMainStack(PushedWidget.Get());
	SuccessRequest->OnPushedToStackRequestCompleted().Broadcast(SuccessRequest->GetRequestId(), PushedWidget);
	PushToStackRequests.RemoveSingleSwap(*SuccessRequest);
}

void UTartarusPrimaryGameLayout::HandleRequestFailed(const FPushToStackRequestInfo* const FailedRequest)
{
	if (!FailedRequest)
	{
		return;
	}

	FailedRequest->OnPushedToStackRequestCompleted().Broadcast(FailedRequest->GetRequestId(), nullptr);
	PushToStackRequests.RemoveSingleSwap(*FailedRequest);
}

void UTartarusPrimaryGameLayout::HandleWidgetClassLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle)
{
	// Get the request that is being handled.
	FPushToStackRequestInfo* const CurrentRequest = PushToStackRequests.FindByPredicate([&ASyncLoadRequestId](const FPushToStackRequestInfo& Request)
		{
			return Request.GetASyncLoadRequestId() == ASyncLoadRequestId;
		});

	if (!CurrentRequest || !CurrentRequest->GetRequestId().IsValid())
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Push to stack failed: Could not find the request!"), __FUNCTION__);
		return;
	}

	TSubclassOf<UCommonActivatableWidget> WidgetClass = Cast<UClass>(AssetHandle.Get()->GetLoadedAsset());

	if (!IsValid(WidgetClass))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Push to stack failed: Class failed to load!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	UCommonActivatableWidget* const NewWidget = MainStack->AddWidget(WidgetClass);

	if (!IsValid(NewWidget))
	{
		UE_LOG(LogTartarus, Warning, TEXT("%s: Push to stack failed: Could not create the widget!"), __FUNCTION__);
		HandleRequestFailed(CurrentRequest);

		return;
	}

	NewWidget->ActivateWidget();

	HandleRequestSuccess(CurrentRequest, NewWidget);
}
#pragma endregion
