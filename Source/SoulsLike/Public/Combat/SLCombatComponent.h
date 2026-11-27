#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLCombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FSLOnWeaponHit, AActor* /*Target*/, const FHitResult& /*Hit*/);

UCLASS()
class SOULSLIKE_API USLCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLCombatComponent();

	void BeginWeaponTrace();
	void EndWeaponTrace();

	FSLOnWeaponHit OnWeaponHit;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CaptureBladePoints(TArray<FVector>& OutPoints) const;
	void TraceStep();

	// Kılıç mesh'inin kendi koordinatlarında: korumadan uca (namlu +Y yönünde)
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	FVector BladeStart = FVector(0.f, 15.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	FVector BladeEnd = FVector(0.f, 112.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	int32 PointCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius = 8.f;

	UPROPERTY(EditAnywhere, Category = "Trace")
	bool bDrawDebug = true;

	bool bTracing = false;
	TArray<FVector> PreviousPoints;
	TSet<TWeakObjectPtr<AActor>> HitActors;
};