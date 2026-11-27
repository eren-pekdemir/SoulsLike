// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLDodgeAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/SLCharacterBase.h"

USLDodgeAbility::USLDodgeAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = true;
}

void USLDodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASLCharacterBase* Character = Cast<ASLCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

	const FVector Input = Character->GetLastMovementInputVector();
	
	if (DirectionalMontages.Num() != 8 || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	int32 Index = 4;                                  
	if (!Input.IsNearlyZero())
	{
		if (!Character->IsStrafing())
		{
			Character->SetActorRotation(Input.Rotation()); 
			Index = 0;                                    
		}
		else
		{
			const float Angle = FMath::FindDeltaAngleDegrees(Character->GetActorRotation().Yaw, Input.Rotation().Yaw);
			Index = (FMath::RoundToInt(Angle / 45.f) + 8) % 8;
		}
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DirectionalMontages[Index]);
	Task->OnCompleted.AddDynamic(this, &USLDodgeAbility::OnMontageFinished);
	Task->OnBlendOut.AddDynamic(this, &USLDodgeAbility::OnMontageFinished);
	Task->OnInterrupted.AddDynamic(this, &USLDodgeAbility::OnMontageFinished);
	Task->OnCancelled.AddDynamic(this, &USLDodgeAbility::OnMontageFinished);
	Task->ReadyForActivation();
}

void USLDodgeAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
