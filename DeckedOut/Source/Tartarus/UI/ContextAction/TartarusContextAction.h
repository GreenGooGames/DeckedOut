// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "TartarusContextAction.generated.h"

class UTartarusContextMenu;

/**
 * 
 */
UCLASS()
class TARTARUS_API UTartarusContextAction : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void SetParentMenu(UTartarusContextMenu* const Menu);
	virtual void ExecuteAction() { return; };
	const FText& GetName() const { return Name; }

protected:
	UPROPERTY(EditDefaultsOnly)
		FText Name = FText();

	TWeakObjectPtr<UTartarusContextMenu> ParentMenu = nullptr;
};
