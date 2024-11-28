// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/TartarusAbilityInteract.h"
#include "Abilities/Tasks/TartarusWaitForInteractTask.h"

void UTartarusAbilityInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	
	UTartarusWaitForInteractTask* const WaitForInteractTask = UTartarusWaitForInteractTask::NewAbilityTask<UTartarusWaitForInteractTask>(this);
	if (!IsValid(WaitForInteractTask))
	{
		return;
	}

	WaitForInteractTask->ReadyForActivation();
}
