// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLGameplayAbility.h"
#include "SLGameplayTags.h"

#include "AbilitySystemComponent.h"

bool USLGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return false;
	
	// Karakter meşgulse başlama; ama iptal penceresi açıksa başla.
	FGameplayTagContainer BusyTags;
	BusyTags.AddTag(SLGameplayTags::State_Attacking);
	BusyTags.AddTag(SLGameplayTags::State_Dodging);
	BusyTags.AddTag(SLGameplayTags::State_HitReact);

	const bool bBusy = ASC->HasAnyMatchingGameplayTags(BusyTags);

	const bool bCancelOpen = ASC->HasMatchingGameplayTag(
		SLGameplayTags::State_Window_Cancel);

	return !bBusy || bCancelOpen;
}
