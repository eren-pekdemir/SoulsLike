// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Animation/SLAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void USLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	if (!Character) return;
	
	GroundSpeed = Character->GetVelocity().Size2D();
	
	Direction = UKismetAnimationLibrary::CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
	
	bIsMoving = GroundSpeed > 3.f;
	
	bIsFalling = Character->GetCharacterMovement()->IsFalling();
	
}
