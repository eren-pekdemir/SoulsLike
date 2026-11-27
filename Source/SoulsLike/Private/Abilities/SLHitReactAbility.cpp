// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLHitReactAbility.h"

#include "SLGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Combat/SLHitDirection.h"

USLHitReactAbility::USLHitReactAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = true;
	
	FAbilityTriggerData Trigger;
	Trigger.TriggerTag = SLGameplayTags::Event_HitReact;
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);
}

void USLHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const AActor* Attacker = TriggerEventData ? TriggerEventData->Instigator.Get() : nullptr;
	const AActor* Victim = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	
	const ESLHitDirection Direction = SLCombat::GetHitDirection(Victim, Attacker);

	UAnimMontage* Montage = SLCombat::PickMontage(Direction, FrontMontages, BackMontages, LeftMontages, RightMontages);
	if (!Montage)
	{
		// Bu yön için montaj atanmamış
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage);
	Task->OnCompleted.AddDynamic(this, &USLHitReactAbility::OnMontageFinished);
	Task->OnBlendOut.AddDynamic(this, &USLHitReactAbility::OnMontageFinished);
	Task->OnInterrupted.AddDynamic(this, &USLHitReactAbility::OnMontageFinished);
	Task->OnCancelled.AddDynamic(this, &USLHitReactAbility::OnMontageFinished);
	Task->ReadyForActivation();
}
void USLHitReactAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}