// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SLDodgeAbility.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLDodgeAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USLDodgeAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	// Saat yönünde: F, F_R45, F_R90, B_R45, B, B_L45, F_L90, F_L45
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TArray<TObjectPtr<UAnimMontage>> DirectionalMontages;
	
	UFUNCTION() void OnMontageFinished();
};
