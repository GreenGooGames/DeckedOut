// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CorrbolgEntryPreview.generated.h"

class UCommonTextBlock;
class UCorrbolgInventoryViewListItem;

/**
 * 
 */
UCLASS()
class CORRBOLG_API UCorrbolgEntryPreview : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPreviewEntry(const UCorrbolgInventoryViewListItem* const ViewListItem);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EntryNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EntryDescriptionText = nullptr;

};
