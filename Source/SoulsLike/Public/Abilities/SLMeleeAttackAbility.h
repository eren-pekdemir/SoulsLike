// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SLMeleeAttackAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class SOULSLIKE_API USLMeleeAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USLMeleeAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TArray<TObjectPtr<UAnimMontage>> ComboMontages;  

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag ComboInputTag;
	
	void PlayStep(int32 Index);

	UFUNCTION() void OnComboInput(FGameplayEventData Payload);
	UFUNCTION() void OnHandoff(FGameplayEventData Payload);
	UFUNCTION() void OnMontageFinished();

	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	int32 ComboIndex = 0;
	bool bNextQueued = false;
};
