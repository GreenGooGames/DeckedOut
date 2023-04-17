// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "System/TartarusASyncLoadData.h"

#include "TartarusPrimaryGameLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

DECLARE_EVENT_TwoParams(UTartarusPrimaryGameLayout, FPushedToStackEvent, FGuid /*RequestId*/, TWeakObjectPtr<UCommonActivatableWidget>/*PushedWidget*/);

USTRUCT()
struct FPushToStackRequestInfo : public FASyncLoadRequest
{
	GENERATED_BODY()

public:
	FPushToStackRequestInfo() {}
	FPushToStackRequestInfo(const FPushedToStackEvent& OnCompletedEvent)
	{
		RequestId = FGuid::NewGuid();

		OnRequestCompletedEvent = OnCompletedEvent;
	}

	const FPushedToStackEvent& OnPushedToStackRequestCompleted() const { return OnRequestCompletedEvent; }

private:
	FPushedToStackEvent OnRequestCompletedEvent = FPushedToStackEvent();
};

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// Removes a widget from the mainstack and deactivate it.
	void PopWidgetFromMainStack(UCommonActivatableWidget& WidgetInstance);

private:
	// The mainstack that widgets will be added to.
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UCommonActivatableWidgetStack> MainStack = nullptr;

	void OnWidgetPushedToMainStack(const UCommonActivatableWidget* const Widget);
	void OnWidgetPoppedFromMainStack();

#pragma region AsyncLoading
public:
	/*
	* Constructs, pushes and activates a new widget to the mainstack.
	* Return: The Guid of the async load request.
	*/
	FGuid AsyncRequestPushToStack(const TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FPushedToStackEvent& OnRequestCompletedEvent);

protected:
	// Notfies the requester that the request has succeeded and removes the request from the queue.
	void HandleRequestSuccess(const FPushToStackRequestInfo* const SuccessRequest, TWeakObjectPtr<UCommonActivatableWidget> PushedWidget);

	// Notifies the requester that the request failed and removes the request from the queue.
	void HandleRequestFailed(const FPushToStackRequestInfo* const FailedRequest);

	// Called when the Widget class is loaded.
	void HandleWidgetClassLoaded(FGuid ASyncLoadRequestId, TSharedPtr<FStreamableHandle> AssetHandle);

private:
	TArray<FPushToStackRequestInfo> PushToStackRequests;
#pragma endregion
};
