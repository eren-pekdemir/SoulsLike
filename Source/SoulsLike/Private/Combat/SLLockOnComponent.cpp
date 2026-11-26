// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SLLockOnComponent.h"

#include "Combat/SLLockOnTargetInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

// Sets default values for this component's properties
USLLockOnComponent::USLLockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USLLockOnComponent::ToggleLockOn()
{
	if (IsLockedOn())
	{
		Target = nullptr;
		return;
	}
	
	Target = FindBestTarget();
}

AActor* USLLockOnComponent::FindBestTarget() const
{	
	
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->GetController()) return nullptr;
	
	FVector CameraLocation;
	FRotator CameraRotation;
	Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> Candidates;
	UKismetSystemLibrary::SphereOverlapActors(this, Pawn->GetActorLocation(), SearchRadius,
		ObjectTypes, nullptr, { Pawn }, Candidates);
	
	AActor* BestActor = nullptr;
	float BestScore = -1.f;
	
	for ( AActor* Actor : Candidates )
	{
		const ISLLockOnTargetInterface* LockTarget = Cast<ISLLockOnTargetInterface>(Actor);
		if (!LockTarget || !LockTarget->IsLockOnAllowed()) continue;
		const FVector TargetLocation = LockTarget->GetLockOnLocation();
		const FVector ToTarget = (TargetLocation - CameraLocation).GetSafeNormal();
		float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraRotation.Vector(), ToTarget)));
		if (Angle > MaxAngle) continue;
		
		float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetLocation);
		
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Pawn);
		Params.AddIgnoredActor(Actor);
		
		GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TargetLocation, ECollisionChannel::ECC_Visibility,Params);
		if (Hit.bBlockingHit) continue;
		
		float Best = 0.7 *	(1 - Angle / MaxAngle) + 0.3 * (1 - Distance / SearchRadius);
		if (Best > BestScore)
		{
			BestScore = Best;
			BestActor = Actor;
		}
	}
	
	return BestActor;
}


// Called when the game starts
void USLLockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USLLockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsLockedOn())
	{
		DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 25, 8, FColor::Red);
	}
}

