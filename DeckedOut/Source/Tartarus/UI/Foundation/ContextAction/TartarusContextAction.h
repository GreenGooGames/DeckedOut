// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusContextAction.generated.h"

class UTartarusContextMenuWidget;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextAction : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void SetParentMenu(UTartarusContextMenuWidget* const Menu);
	virtual void ExecuteAction() { return; };
	const FText& GetName() const { return Name; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FText Name = FText();

	TWeakObjectPtr<UTartarusContextMenuWidget> ParentMenu = nullptr;
};
