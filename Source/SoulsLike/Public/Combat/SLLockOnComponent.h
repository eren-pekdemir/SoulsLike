// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLLockOnComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSLOnLockOnChanged, AActor*);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSLIKE_API USLLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLLockOnComponent();
	
	FSLOnLockOnChanged OnLockOnChanged;
	
	void ToggleLockOn();

	bool IsLockedOn() const {return Target.IsValid();}
	AActor* GetTarget() const { return Target.Get();}

private:
	AActor* FindBestTarget() const;
	void SetTarget(AActor* NewTarget);
	bool HasLineOfSight(const FVector& From, const AActor* Candidate, const FVector& To) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float SearchRadius = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float MaxAngle = 45.f;
	
	TWeakObjectPtr<AActor> Target;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float BreakDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float LostSightTime = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float CameraInterpSpeed = 8.f;

	float TimeOutOfSight = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float PitchOffset = -15.f;
	
	bool ShouldBreakLock(float DeltaTime);
	void UpdateCamera(float DeltaTime) const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
