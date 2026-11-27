// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLAnimNotify_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category = "Event")
	FGameplayTag EventTag;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
