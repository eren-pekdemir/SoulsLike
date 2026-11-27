// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/SLCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/SLAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayEffect.h"
#include "SLGameplayTags.h"
#include "Combat/SLCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Combat/SLHitDirection.h"


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
	
	Combat = CreateDefaultSubobject<USLCombatComponent>(TEXT("Combat"));
}

void ASLCharacterBase::HandleDamage(float Damage, AActor* DamageInstigator)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s -%.0f (kalan %.0f)"), *GetName(), Damage, Attributes->GetHealth()));
	
	if (Attributes->GetHealth() <= 0.0f)
	{
		if (!IsDead()) Die(DamageInstigator);
		return;   // ölen karaktere sarsılma event'i gitmesin
	}

	// GA_HitReact bu event'le kendiliğinden başlar (yeteneğin trigger'ı)
	FGameplayEventData Payload;
	Payload.Instigator = DamageInstigator;
	Payload.EventMagnitude = Damage;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, SLGameplayTags::Event_HitReact, Payload);
}

bool ASLCharacterBase::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(SLGameplayTags::State_Dead);
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

void ASLCharacterBase::Die(AActor* Killer)
{
	AbilitySystem->CancelAllAbilities();

	AbilitySystem->AddLooseGameplayTag(SLGameplayTags::State_Dead);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	const ESLHitDirection Direction = SLCombat::GetHitDirection(this, Killer);
	if (UAnimMontage* DeathMontage = SLCombat::PickMontage(Direction,
		DeathFrontMontages, DeathBackMontages, DeathLeftMontages, DeathRightMontages))
	{
		// Montajda "Enable Auto Blend Out" kapalı olmalı, yoksa ölü ayağa kalkar
		PlayAnimMontage(DeathMontage);
		return;
	}

	// Ölüm montajı atanmamışsa yedek: ragdoll
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}
