// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SLHitReactAbility.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLHitReactAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USLHitReactAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitReact") TArray<TObjectPtr<UAnimMontage>> FrontMontages;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact") TArray<TObjectPtr<UAnimMontage>> BackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact") TArray<TObjectPtr<UAnimMontage>> LeftMontages;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact") TArray<TObjectPtr<UAnimMontage>> RightMontages;
	UFUNCTION() void OnMontageFinished();
};
