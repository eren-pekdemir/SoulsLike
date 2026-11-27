// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SLAnimNotifyState_WeaponTrace.h"

#include "Combat/SLCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

void USLAnimNotifyState_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp) return;
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	USLCombatComponent* CombatComp = Owner->FindComponentByClass<USLCombatComponent>();
	if (CombatComp)
	{
		CombatComp->BeginWeaponTrace();
	}
}

void USLAnimNotifyState_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	USLCombatComponent* CombatComp = Owner->FindComponentByClass<USLCombatComponent>();
	if (CombatComp)
	{
		CombatComp->EndWeaponTrace();
	}
	
}
