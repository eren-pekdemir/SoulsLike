// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/SLCombatComponent.h"

#include "Characters/SLCharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USLCombatComponent::USLCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;

	// ...
}

void USLCombatComponent::BeginWeaponTrace()
{
	HitActors.Empty();
	CaptureBladePoints(PreviousPoints);
	bTracing = true;
}

void USLCombatComponent::EndWeaponTrace()
{
	bTracing = false;
}


// Called every frame
void USLCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTracing)
	{
		TraceStep();
	}
}

void USLCombatComponent::CaptureBladePoints(TArray<FVector>& OutPoints) const
{
	OutPoints.Reset();
	ASLCharacterBase* Character = Cast<ASLCharacterBase>(GetOwner());
	if (Character)
	{
		UStaticMeshComponent* WeaponMesh = Character->GetWeaponMesh();
		if (!WeaponMesh) return;
		
		if (PointCount < 2) return;
		
		FTransform WeaponTransform = WeaponMesh->GetComponentTransform();
		
		for (int32 i = 0; i < PointCount; ++i)
		{
			const float t = static_cast<float>(i) / (PointCount - 1);
			OutPoints.Add(WeaponTransform.TransformPosition(FMath::Lerp(BladeStart, BladeEnd, t)));
		}
	}
}

void USLCombatComponent::TraceStep()
{
	TArray<FVector> CurrentPoints;
	CaptureBladePoints(CurrentPoints);
	if (CurrentPoints.Num() != PreviousPoints.Num())
	{
		PreviousPoints = CurrentPoints;
		return;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	const FCollisionObjectQueryParams ObjectParams(ECC_Pawn);
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	for (int32 i = 0; i < CurrentPoints.Num(); ++i)
	{
		TArray<FHitResult> Hits;
		GetWorld()->SweepMultiByObjectType(Hits, PreviousPoints[i], CurrentPoints[i],
			FQuat::Identity, ObjectParams, Sphere, Params);

		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), PreviousPoints[i], CurrentPoints[i], FColor::Yellow, false, 1.f);
		}

		for (const FHitResult& Hit : Hits)
		{
			AActor* Target = Hit.GetActor();
			if (!Target || HitActors.Contains(Target)) continue;

			HitActors.Add(Target);
			OnWeaponHit.Broadcast(Target, Hit);

			if (bDrawDebug)
			{
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 15.f, 8, FColor::Red, false, 1.f);
			}
		}
	}

	PreviousPoints = CurrentPoints;
}

