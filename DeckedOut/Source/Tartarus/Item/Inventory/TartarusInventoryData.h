// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Misc/EnumRange.h"
#include "System/TartarusHelpers.h"
#include "GameplayTagContainer.h"

#include "TartarusInventoryData.generated.h"

class UTartarusItem;

UENUM()
enum class EInventoryChanged : uint8
{
	Stored,
	Retrieved
};

USTRUCT()
struct FInventoryStackId
{
	GENERATED_BODY()

public:
	FInventoryStackId() {};
	FInventoryStackId(const FGameplayTag& OwningInventoryId)
	{
		StackId = FGuid::NewGuid();
		SubInventoryId = OwningInventoryId;
	}

	const FGuid& GetStackId() const { return StackId; }
	const FGameplayTag& GetInventoryId() const { return SubInventoryId; }

	bool IsValid() const
	{
		return StackId.IsValid() && SubInventoryId != FGameplayTag::EmptyTag;
	}

	void Invalidate()
	{
		StackId.Invalidate();
		SubInventoryId = FGameplayTag::EmptyTag;
	}

	bool operator==(const FInventoryStackId& rhs) const { return GetStackId() == rhs.GetStackId(); }
	bool operator!=(const FInventoryStackId& rhs) const { return GetStackId() != rhs.GetStackId(); }

private:
	FGuid StackId = FGuid();
	FGameplayTag SubInventoryId = FGameplayTag::EmptyTag;
};

USTRUCT()
struct FInventoryStack
{
	GENERATED_BODY()

public:
	FInventoryStack() {}
	FInventoryStack(const FGameplayTag InventoryId, const FPrimaryAssetId NewEntryId, const int32 NewStackSize);

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
	FSubInventory(const FGameplayTag& NewInventoryId, const int32 InventorySize)
	{
		SubInventoryId = NewInventoryId;
		Content.SetNum(InventorySize);
	}

	FInventoryStackId AddEntry(const FPrimaryAssetId EntryId, const bool bTryStack, const int32 StackSize);
	bool RemoveEntry(const FInventoryStackId& StackId, const int32 StackSize);

	const FInventoryStackId* const FindStackId(const FPrimaryAssetId EntryId) const;
	const TArray<FInventoryStack>& GetContents() const { return Content; }
	const FInventoryStack* FindStack(const FInventoryStackId& StackId) const;
	void Sort(TArray<UTartarusItem*> ItemsData);

protected:
	FInventoryStack* FindEditableStack(const FInventoryStackId& StackId);

private:
	FGameplayTag SubInventoryId = FGameplayTag::EmptyTag;
	TArray<FInventoryStack> Content;
};
