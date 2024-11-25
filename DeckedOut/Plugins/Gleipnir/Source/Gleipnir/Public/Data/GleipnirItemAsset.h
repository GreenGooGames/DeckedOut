// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"

#include "GleipnirItemAsset.generated.h"

/**
 * 
 */
UCLASS()
class GLEIPNIR_API UGleipnirItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** Name of the item. */
	UPROPERTY(EditDefaultsOnly, Category = "General")
	FText ItemName = FText();

	// Description to provide information about the item.
	UPROPERTY(EditDefaultsOnly, Category = "General")
	FText ItemDescription = FText();

	// The type of the item.
	UPROPERTY(EditDefaultsOnly, Category = "General")
	FGameplayTag ItemTypeTag = FGameplayTag::EmptyTag;

	// Reference texture to represent the item in UI.
	UPROPERTY(EditDefaultsOnly, Category = "Representation")
	TSoftObjectPtr<UTexture2D> ItemDisplayTexture = nullptr;

	/** The actor to represent this item in the world. */
	UPROPERTY(EditDefaultsOnly, Category = "Representation")
	TSoftClassPtr<AActor> ItemInstanceClass = nullptr;
};
