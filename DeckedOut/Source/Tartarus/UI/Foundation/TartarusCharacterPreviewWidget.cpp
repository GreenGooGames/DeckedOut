// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusCharacterPreviewWidget.h"

#include "CommonLazyImage.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine//SceneCapture2D.h"
#include "Engine/World.h"
#include "Logging/TartarusLogChannels.h"
#include "Player/TartarusPlayerCharacter.h"
#include "Player/TartarusPlayerController.h"

void UTartarusCharacterPreviewWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CreateSceneCapture();
}

void UTartarusCharacterPreviewWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	AssociateSceneCapture();
}

void UTartarusCharacterPreviewWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	DissociateSceneCapture();
}

void UTartarusCharacterPreviewWidget::CreateSceneCapture()
{
	// Get the player
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to setup SceneCapture: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	ATartarusPlayerCharacter* const PlayerCharacter = PlayerController->GetPawn<ATartarusPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to setup SceneCapture: No player Character!"), *FString(__FUNCTION__));
		return;
	}

	// If needed create a new instance of the CaptureComponent.
	if (!IsValid(SceneCapture2DInstance))
	{
		SceneCapture2DInstance = GetWorld()->SpawnActor<ASceneCapture2D>(SceneCapture2DTemplate.LoadSynchronous(), PlayerCharacter->GetActorTransform());
		if (!IsValid(SceneCapture2DInstance))
		{
			UE_LOG(LogTartarus, Log, TEXT("%s: Failed to setup SceneCapture: Could not instantiate a capture actor!"), *FString(__FUNCTION__));
			return;
		}
	}

	// Set the Capture Component to only render certain components.
	SceneCapture2DInstance->GetCaptureComponent2D()->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void UTartarusCharacterPreviewWidget::AssociateSceneCapture()
{
	// Get the player
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Associate SceneCapture Actor: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	ATartarusPlayerCharacter* const PlayerCharacter = PlayerController->GetPawn<ATartarusPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Associate SceneCapture Actor: No player Character!"), *FString(__FUNCTION__));
		return;
	}

	// validate the sceneCaptureActor isntance.
	if (!IsValid(SceneCapture2DInstance))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Associate SceneCapture Actor: No SceneCaptureActor!"), *FString(__FUNCTION__));
		return;
	}

	// Transform the actor to be looking at the player.
	SceneCapture2DInstance->SetActorTransform(PlayerCharacter->GetActorTransform());

	// Add the player TP character mesh to the list of components to render.
	SceneCapture2DInstance->GetCaptureComponent2D()->ShowOnlyComponent(PlayerCharacter->GetMesh());

	// TODO: Also show all attached meshes. ie: all equippables equipped.

	// Make sure the mesh is visible.
	if (PlayerCharacter->IsInFirstPerson())
	{
		PlayerCharacter->GetMesh()->SetHiddenInGame(false);
		PlayerCharacter->GetMesh()->bOwnerNoSee = true;
	}
}

void UTartarusCharacterPreviewWidget::DissociateSceneCapture()
{
	// Get the player
	const ATartarusPlayerController* const PlayerController = GetOwningPlayer<ATartarusPlayerController>();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Dissociate SceneCapture Actor: No player controller!"), *FString(__FUNCTION__));
		return;
	}

	ATartarusPlayerCharacter* const PlayerCharacter = PlayerController->GetPawn<ATartarusPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Dissociate SceneCapture Actor: No player Character!"), *FString(__FUNCTION__));
		return;
	}

	// validate the sceneCaptureActor isntance.
	if (!IsValid(SceneCapture2DInstance))
	{
		UE_LOG(LogTartarus, Log, TEXT("%s: Failed to Dissociate SceneCapture Actor: No SceneCaptureActor!"), *FString(__FUNCTION__));
		return;
	}

	// Add the player TP character mesh to the list of components to render.
	SceneCapture2DInstance->GetCaptureComponent2D()->RemoveShowOnlyComponent(PlayerCharacter->GetMesh());

	// Make sure the mesh is invisible if the player is in 1P.
	if (PlayerCharacter->IsInFirstPerson())
	{
		PlayerCharacter->GetMesh()->SetHiddenInGame(true);
		PlayerCharacter->GetMesh()->bOwnerNoSee = false;
	}
}