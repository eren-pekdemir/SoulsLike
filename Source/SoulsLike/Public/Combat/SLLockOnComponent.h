// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Components/ActorComponent.h"
#include "SLLockOnComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSLIKE_API USLLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLLockOnComponent();
	
	void ToggleLockOn();
	
	bool IsLockedOn() const {return Target.IsValid();}
	AActor* GetTarget() const { return Target.Get();}
	
private:
	AActor* FindBestTarget() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float SearchRadius = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float MaxAngle = 45.f;
	
	TWeakObjectPtr<AActor> Target;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
