// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SLLockOnComponent.h"

#include "Combat/SLLockOnTargetInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USLLockOnComponent::USLLockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	// ...
}

void USLLockOnComponent::ToggleLockOn()
{
	if (IsLockedOn())
	{
		SetTarget(nullptr);
		return;
	}
	SetTarget(FindBestTarget());
}

void USLLockOnComponent::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
	
	TimeOutOfSight = 0.f;
	
	OnLockOnChanged.Broadcast(NewTarget);
}

bool USLLockOnComponent::HasLineOfSight(const FVector& From, const AActor* Candidate, const FVector& To) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(Candidate);
		
	return !GetWorld()->LineTraceSingleByChannel(Hit, From, To, ECollisionChannel::ECC_Visibility,Params);
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
		
		if (!HasLineOfSight(CameraLocation, Actor, TargetLocation)) continue;
		
		float Best = 0.7 *	(1 - Angle / MaxAngle) + 0.3 * (1 - Distance / SearchRadius);
		if (Best > BestScore)
		{
			BestScore = Best;
			BestActor = Actor;
		}
	}
	
	return BestActor;
}


bool USLLockOnComponent::ShouldBreakLock(float DeltaTime)
{
	const ISLLockOnTargetInterface* LockTarget = Cast<ISLLockOnTargetInterface>(Target.Get());
	if (!LockTarget || !LockTarget->IsLockOnAllowed()) return true;
	
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	
	float Distance = FVector::Dist(OwnerLocation, TargetLocation);
	if (Distance > BreakDistance) return true;
	
	if (!HasLineOfSight(OwnerLocation,Target.Get(), TargetLocation))
	{
		TimeOutOfSight += DeltaTime;
	}
	else
	{
		TimeOutOfSight = 0.f;
	}
	
	if (TimeOutOfSight > LostSightTime) return true;
	
	return false;
}

void USLLockOnComponent::UpdateCamera(float DeltaTime) const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	AController* Controller = Pawn->GetController();
	if (!Controller) return;

	const ISLLockOnTargetInterface* LockTarget = Cast<ISLLockOnTargetInterface>(Target.Get());
	if (!LockTarget) return;

	FVector From = Pawn->GetActorLocation();
	FVector To = LockTarget->GetLockOnLocation();
	
	float Dist = FVector::Dist(From, To);
	if (Dist < 1.f)
	{
		return;
	}
	
	FRotator Desired = UKismetMathLibrary::FindLookAtRotation(From,To);
	
	const FRotator ClampedDesired(FMath::Clamp(Desired.Pitch + PitchOffset, -35.f, 10.f), Desired.Yaw, 0.f);
	
	FRotator Interp = UKismetMathLibrary::RInterpTo(Controller->GetControlRotation(),ClampedDesired,DeltaTime, CameraInterpSpeed);
	Controller->SetControlRotation(Interp);
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

	if (!IsLockedOn()) return; 

	if (ShouldBreakLock(DeltaTime))     
	{
		SetTarget(nullptr);
		return;
	}

	UpdateCamera(DeltaTime);

	if (const ISLLockOnTargetInterface* LockTarget = Cast<ISLLockOnTargetInterface>(Target.Get()))
	{
		DrawDebugSphere(GetWorld(), LockTarget->GetLockOnLocation(), 25.f, 8, FColor::Red);
	}
}

