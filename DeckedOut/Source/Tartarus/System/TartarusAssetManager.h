// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"

#include "TartarusAssetManager.generated.h"

DECLARE_EVENT_TwoParams(UTartarusAssetManager, FAsyncLoadAssetRequestCompletedEvent, FGuid /*RequestId*/, TSharedPtr<FStreamableHandle> /*LoadedAsset*/);

USTRUCT()
struct FAsyncLoadAssetRequest
{
	GENERATED_BODY()

public:
	TSharedPtr<FStreamableHandle> AssetHandle = nullptr;
	FAsyncLoadAssetRequestCompletedEvent RequestCompletedEvent;

	const FGuid& GetRequestId() const { return UniqueId; }

private:
	FGuid UniqueId = FGuid::NewGuid();
};

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	// Returns the AssetManager singleton object.
	static UTartarusAssetManager& Get();

#pragma region AsyncAssetLoading
public:
	/*
	* Loads an asset and calls event when completed.
	* Return: the unique ID of the request.	
	*/
	FGuid AsyncRequestLoadAsset(const FSoftObjectPath& TargetToLoad, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent);
	FGuid AsyncRequestLoadAsset(const FPrimaryAssetId& AssetId, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent);
	FGuid AsyncRequestLoadAssets(TArray<FSoftObjectPath> TargetsToLoad, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent);
	FGuid AsyncRequestLoadAssets(TArray<FPrimaryAssetId> AssetIds, const FAsyncLoadAssetRequestCompletedEvent& OnRequestCompletedEvent);

	// Cancels the request matching the RequestId.
	void CancelRequest(const FGuid& RequestId);

protected:
	// Callback to when any asset is loaded.
	void HandleAssetLoaded();

	TArray<FAsyncLoadAssetRequest> AsyncLoadRequests;
#pragma endregion
};
