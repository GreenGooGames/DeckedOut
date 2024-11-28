// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/TartarusAbilityInteract.h"
#include "Abilities/TaratusWaitForInteractTask.h"

void UTartarusAbilityInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	
	UTaratusWaitForInteractTask* const WaitForInteractTask = UTaratusWaitForInteractTask::NewAbilityTask<UTaratusWaitForInteractTask>(this);
	if (!IsValid(WaitForInteractTask))
	{
		return;
	}

	WaitForInteractTask->ReadyForActivation();
}
