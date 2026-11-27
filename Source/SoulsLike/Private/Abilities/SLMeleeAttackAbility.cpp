// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLMeleeAttackAbility.h"
#include "SLGameplayTags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/SLAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Combat/SLCombatComponent.h"

USLMeleeAttackAbility::USLMeleeAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USLMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (Steps.IsEmpty() || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	USLCombatComponent* CombatComp = GetAvatarActorFromActorInfo()->FindComponentByClass<USLCombatComponent>();
	if (CombatComp)
	{
		WeaponHitHandle = CombatComp->OnWeaponHit.AddUObject(this, &USLMeleeAttackAbility::OnWeaponHit);
	}

	ComboIndex = 0;
	PlayStep(0);

	UAbilityTask_WaitGameplayEvent* InputTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ComboInputTag);
	InputTask->EventReceived.AddDynamic(this, &USLMeleeAttackAbility::OnComboInput);
	InputTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* HandoffTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, SLGameplayTags::Event_Combo_Handoff);
	HandoffTask->EventReceived.AddDynamic(this, &USLMeleeAttackAbility::OnHandoff);
	HandoffTask->ReadyForActivation();
}

void USLMeleeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (USLCombatComponent* CombatComp = Avatar->FindComponentByClass<USLCombatComponent>())
		{
			CombatComp->OnWeaponHit.Remove(WeaponHitHandle);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		
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

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Steps[Index].Montage);
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
	if (ComboIndex + 1 >= Steps.Num()) return;         // zincirin sonu
	if (!CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo)) return;

	ComboIndex++;
	PlayStep(ComboIndex);
}

void USLMeleeAttackAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USLMeleeAttackAbility::OnWeaponHit(AActor* Target, const FHitResult& Hit)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!TargetASC || !DamageEffect) return;

	if (TargetASC->HasMatchingGameplayTag(SLGameplayTags::State_Invulnerable) ||
		TargetASC->HasMatchingGameplayTag(SLGameplayTags::State_Dead))
	{
		return;                                
	}

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffect);
	Spec.Data->SetSetByCallerMagnitude(SLGameplayTags::Data_Damage, Steps[ComboIndex].Damage);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	
}

