// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SLAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Characters/SLCharacterBase.h"

USLAttributeSet::USLAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
}

void USLAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void USLAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void USLAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetIncomingDamageAttribute()) return;

	const float Damage = GetIncomingDamage();
	SetIncomingDamage(0.f);                       // meta attribute: oku ve sıfırla
	if (Damage <= 0.f) return;

	SetHealth(FMath::Clamp(GetHealth() - Damage, 0.f, GetMaxHealth()));

	if (ASLCharacterBase* Character = Cast<ASLCharacterBase>(GetOwningActor()))
	{
		AActor* Instigator = Data.EffectSpec.GetContext().GetOriginalInstigator();
		Character->HandleDamage(Damage, Instigator);
	}
}