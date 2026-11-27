// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLMeleeAttackAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/SLAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USLMeleeAttackAbility::USLMeleeAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USLMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ComboMontages.IsEmpty() || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ComboIndex = 0;
	PlayStep(0);

	UAbilityTask_WaitGameplayEvent* InputTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ComboInputTag);
	InputTask->EventReceived.AddDynamic(this, &USLMeleeAttackAbility::OnComboInput);
	InputTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* HandoffTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Handoff")));
	HandoffTask->EventReceived.AddDynamic(this, &USLMeleeAttackAbility::OnHandoff);
	HandoffTask->ReadyForActivation();
}

bool USLMeleeAttackAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const USLAttributeSet* Set = ActorInfo->AbilitySystemComponent->GetSet<USLAttributeSet>();
	if (Set && Set->GetStamina() > 0.f)
	{
		return true;
	}
	
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void USLMeleeAttackAbility::PlayStep(int32 Index)
{
	bNextQueued = false;
	
	if (MontageTask)
	{
		MontageTask->EndTask();
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontages[Index]);
	MontageTask->OnCompleted.AddDynamic(this, &USLMeleeAttackAbility::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &USLMeleeAttackAbility::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &USLMeleeAttackAbility::OnMontageFinished);
	MontageTask->OnCancelled.AddDynamic(this, &USLMeleeAttackAbility::OnMontageFinished);
	MontageTask->ReadyForActivation();
}

void USLMeleeAttackAbility::OnComboInput(FGameplayEventData Payload)
{
	bNextQueued = true;
}

void USLMeleeAttackAbility::OnHandoff(FGameplayEventData Payload)
{
	if (!bNextQueued) return;                                  // tıklama yoksa devam etme
	if (ComboIndex + 1 >= ComboMontages.Num()) return;         // zincirin sonu
	if (!CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo)) return;

	ComboIndex++;
	PlayStep(ComboIndex);
}

void USLMeleeAttackAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

