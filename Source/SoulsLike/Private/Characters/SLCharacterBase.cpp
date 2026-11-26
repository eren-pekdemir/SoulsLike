// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/SLCharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ASLCharacterBase::ASLCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket_R"));
}

// Called when the game starts or when spawned
void ASLCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}
