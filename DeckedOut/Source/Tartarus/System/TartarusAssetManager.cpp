// Fill out your copyright notice in the Description page of Project Settings.


#include "System/TartarusAssetManager.h"

#include "Engine/Engine.h"
#include "Logging/TartarusLogChannels.h"

UTartarusAssetManager& UTartarusAssetManager::Get()
{
	check(GEngine);

	if (UTartarusAssetManager* Singleton = Cast<UTartarusAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogTartarus, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to TartarusAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UTartarusAssetManager>();
}

#pragma region AsyncAssetLoading
FGuid UTartarusAssetManager::AsyncRequestLoadAsset(const FSoftObjectPath& TargetToLoad, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent)
{
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(TargetToLoad, FStreamableDelegate::CreateUObject(this, &UTartarusAssetManager::HandleAssetLoaded));
	
	if (!Handle.IsValid())
	{
		UE_LOG(LogTartarus, Fatal, TEXT("%s: Request failed: Could not start loading the asset!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FAsyncLoadAssetRequest Request = FAsyncLoadAssetRequest();
	Request.AssetHandle = Handle;
	Request.RequestCompletedEvent = OnRequestCompletedEvent;

	AsyncLoadRequests.Add(Request);

	return Request.GetRequestId();
}

FGuid UTartarusAssetManager::AsyncRequestLoadAsset(const FPrimaryAssetId& AssetId, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent)
{
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAsset(AssetId, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UTartarusAssetManager::HandleAssetLoaded));

	if (!Handle.IsValid())
	{
		UE_LOG(LogTartarus, Fatal, TEXT("%s: Request failed: Could not start loading the asset!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FAsyncLoadAssetRequest Request = FAsyncLoadAssetRequest();
	Request.AssetHandle = Handle;
	Request.RequestCompletedEvent = OnRequestCompletedEvent;

	AsyncLoadRequests.Add(Request);

	return Request.GetRequestId();
}

FGuid UTartarusAssetManager::AsyncRequestLoadAssets(TArray<FSoftObjectPath> TargetsToLoad, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent)
{
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(TargetsToLoad, FStreamableDelegate::CreateUObject(this, &UTartarusAssetManager::HandleAssetLoaded));

	if (!Handle.IsValid())
	{
		UE_LOG(LogTartarus, Fatal, TEXT("%s: Request failed: Could not start loading the asset!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FAsyncLoadAssetRequest Request = FAsyncLoadAssetRequest();
	Request.AssetHandle = Handle;
	Request.RequestCompletedEvent = OnRequestCompletedEvent;

	AsyncLoadRequests.Add(Request);

	return Request.GetRequestId();
}

FGuid UTartarusAssetManager::AsyncRequestLoadAssets(TArray<FPrimaryAssetId> AssetIds, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent)
{
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UTartarusAssetManager::HandleAssetLoaded));

	if (!Handle.IsValid())
	{
		UE_LOG(LogTartarus, Fatal, TEXT("%s: Request failed: Could not start loading the asset!"), *FString(__FUNCTION__));
		return FGuid();
	}

	FAsyncLoadAssetRequest Request = FAsyncLoadAssetRequest();
	Request.AssetHandle = Handle;
	Request.RequestCompletedEvent = OnRequestCompletedEvent;

	AsyncLoadRequests.Add(Request);

	return Request.GetRequestId();
}

void UTartarusAssetManager::CancelRequest(const FGuid& RequestId)
{
	for (auto It = AsyncLoadRequests.CreateIterator(); It; ++It)
	{
		FAsyncLoadAssetRequest& CurrentRequest = AsyncLoadRequests[It.GetIndex()];

		if (CurrentRequest.GetRequestId() == RequestId)
		{
			CurrentRequest.AssetHandle.Get()->CancelHandle();

			It.RemoveCurrent();
		}
	}

}

void UTartarusAssetManager::HandleAssetLoaded()
{
	for (auto It = AsyncLoadRequests.CreateIterator(); It; ++It)
	{
		FAsyncLoadAssetRequest& CurrentRequest = AsyncLoadRequests[It.GetIndex()];

		if (CurrentRequest.AssetHandle->HasLoadCompleted())
		{
			// Notify the requester that the asset is loaded.
			CurrentRequest.RequestCompletedEvent.Broadcast(CurrentRequest.GetRequestId(), CurrentRequest.AssetHandle);

			It.RemoveCurrent();
		}
	}
}
#pragma endregion
