// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "SLCharacterBase.generated.h"


class USLAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class USLCombatComponent;

UCLASS(Abstract)
class SOULSLIKE_API ASLCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASLCharacterBase();
	
	virtual bool IsStrafing() const { return false; }
	
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	virtual void HandleDamage(float Damage, AActor* DamageInstigator);
	bool IsDead() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USLAttributeSet> Attributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USLCombatComponent> Combat;
	
	virtual void Die();

	
};
