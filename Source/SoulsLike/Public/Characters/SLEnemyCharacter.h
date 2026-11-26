// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLCharacterBase.h"
#include "Combat/SLLockOnTargetInterface.h"
#include "SLEnemyCharacter.generated.h"

UCLASS()
class SOULSLIKE_API ASLEnemyCharacter : public ASLCharacterBase, public ISLLockOnTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASLEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FVector GetLockOnLocation() const override;
	
	bool IsLockOnAllowed() const override;
	
};
