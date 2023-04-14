// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TartarusInventoryData.generated.h"

UENUM()
enum class EInventoryChanged : uint8
{
	Stored,
	Retrieved
};

UENUM()
enum class EInventoryType : uint8
{
	Compass,
	Artifact,
	Card,
	MAX			UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EInventoryType, EInventoryType::MAX);

USTRUCT()
struct FInventoryStackId
{
	GENERATED_BODY()

public:
	FInventoryStackId() {};
	FInventoryStackId(const EInventoryType OwningInventoryId)
	{
		StackId = FGuid::NewGuid();
		InventoryId = OwningInventoryId;
	}

	const FGuid& GetStackId() const { return StackId; }
	const EInventoryType& GetInventoryId() const { return InventoryId; }

	bool IsValid() const
	{
		return StackId.IsValid() && InventoryId != EInventoryType::MAX;
	}
	void Invalidate()
	{
		StackId.Invalidate();
		InventoryId = EInventoryType::MAX;
	}

	bool operator==(const FInventoryStackId& rhs) const { return GetStackId() == rhs.GetStackId(); }
	bool operator!=(const FInventoryStackId& rhs) const { return GetStackId() != rhs.GetStackId(); }

private:
	FGuid StackId = FGuid();
	EInventoryType InventoryId = EInventoryType::MAX;
};

USTRUCT()
struct FInventoryStack
{
	GENERATED_BODY()

public:
	FInventoryStack() {}
	FInventoryStack(const EInventoryType InventoryId, const FPrimaryAssetId NewEntryId, const int32 NewStackSize);

	//bool operator==(const FInventoryStack& rhs) { return GetStackId() == rhs.GetStackId(); }

	int32 StackSize = 0;
	
	const FInventoryStackId& GetStackId() const { return StackId; }
	FPrimaryAssetId GetEntryId() const { return EntryId; }

private:
	FInventoryStackId StackId = FInventoryStackId();
	FPrimaryAssetId EntryId = FTartarusHelpers::InvalidItemId;
};

USTRUCT()
struct FSubInventory
{
	GENERATED_BODY()

public:
	FSubInventory() {}
	FSubInventory(const EInventoryType NewInventoryId, const int32 InventorySize)
	{
		InventoryId = NewInventoryId;
		Content.SetNum(InventorySize);
	}

	FInventoryStackId AddEntry(const FPrimaryAssetId EntryId, const bool bTryStack, const int32 StackSize);
	bool RemoveEntry(const FInventoryStackId& StackId, const int32 StackSize);

	const FInventoryStackId* const FindStackId(const FPrimaryAssetId EntryId) const;
	const TArray<FInventoryStack>& GetContents() const { return Content; }
	const FInventoryStack* FindStack(const FInventoryStackId& StackId) const;

protected:
	FInventoryStack* FindEditableStack(const FInventoryStackId& StackId);

private:
	EInventoryType InventoryId = EInventoryType::MAX;
	TArray<FInventoryStack> Content;
};
