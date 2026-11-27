// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLGameplayAbility.h"
#include "SLMeleeAttackAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;

USTRUCT(BlueprintType)
struct FSLAttackStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 25.f;
};

UCLASS()
class SOULSLIKE_API USLMeleeAttackAbility : public USLGameplayAbility
{
	GENERATED_BODY()
public:
	USLMeleeAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TArray<FSLAttackStep> Steps;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffect; 

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag ComboInputTag;
	
	void PlayStep(int32 Index);

	UFUNCTION() void OnComboInput(FGameplayEventData Payload);
	UFUNCTION() void OnHandoff(FGameplayEventData Payload);
	UFUNCTION() void OnMontageFinished();
	void OnWeaponHit(AActor* Target, const FHitResult& Hit);
	FDelegateHandle WeaponHitHandle;

	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	int32 ComboIndex = 0;
	bool bNextQueued = false;
};
