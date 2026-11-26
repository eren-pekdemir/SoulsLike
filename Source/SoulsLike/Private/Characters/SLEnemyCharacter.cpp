// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/SLEnemyCharacter.h"


// Sets default values
ASLEnemyCharacter::ASLEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASLEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ASLEnemyCharacter::GetLockOnLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f , 40.f);
}

bool ASLEnemyCharacter::IsLockOnAllowed() const
{
	return true;
}

