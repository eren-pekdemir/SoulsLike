// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/SLCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/SLAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayEffect.h"


// Sets default values
ASLCharacterBase::ASLCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket_R"));
	
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	Attributes = CreateDefaultSubobject<USLAttributeSet>(TEXT("Attributes"));
	
}

// Called when the game starts or when spawned
void ASLCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystem->InitAbilityActorInfo(this, this);
	
	for (auto AbilityClass : StartupAbilities)
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	}
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		if (!EffectClass) continue;

		const FGameplayEffectSpecHandle Spec = AbilitySystem->MakeOutgoingSpec(EffectClass, 1.f, AbilitySystem->MakeEffectContext());
		if (Spec.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
	
	
}

UAbilitySystemComponent* ASLCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}
