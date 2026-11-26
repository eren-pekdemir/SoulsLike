// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsMoving = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsStrafing = false;
	
	
};
