// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TartarusASyncLoadData.Generated.h"

USTRUCT()
struct FASyncLoadRequest
{
	GENERATED_BODY()

public:
	const FGuid& GetRequestId() const { return RequestId; }

	// Set a reference to the Id of the object that the request is loading.
	void SetASyncLoadRequestId(FGuid ActiveAsyncLoadRequestId) { AsyncLoadRequestId = ActiveAsyncLoadRequestId; }
	FGuid GetASyncLoadRequestId() const { return AsyncLoadRequestId; }

	bool operator==(const FASyncLoadRequest& Other) const { return RequestId == Other.GetRequestId(); }

protected:
	// The unique Id of this request.
	FGuid RequestId = FGuid();

	// The Id of an underlying system request.
	FGuid AsyncLoadRequestId = FGuid();
};