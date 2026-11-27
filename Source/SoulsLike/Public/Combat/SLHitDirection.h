#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UAnimMontage;

enum class ESLHitDirection : uint8 { Front, Back, Left, Right };

namespace SLCombat
{
	// Saldıran, kurbana göre hangi yönde?
	inline ESLHitDirection GetHitDirection(const AActor* Victim, const AActor* Attacker)
	{
		if (!Victim || !Attacker) return ESLHitDirection::Front;

		const FVector ToAttacker = (Attacker->GetActorLocation() - Victim->GetActorLocation()).GetSafeNormal2D();
		const float Angle = FMath::FindDeltaAngleDegrees(Victim->GetActorRotation().Yaw, ToAttacker.Rotation().Yaw);

		if (FMath::Abs(Angle) <= 45.f) return ESLHitDirection::Front;
		if (FMath::Abs(Angle) >= 135.f) return ESLHitDirection::Back;
		return Angle > 0.f ? ESLHitDirection::Right : ESLHitDirection::Left;
	}

	// Yöne ait listeden rastgele bir montaj. Liste boşsa nullptr.
	inline UAnimMontage* PickMontage(ESLHitDirection Direction,
		const TArray<TObjectPtr<UAnimMontage>>& Front, const TArray<TObjectPtr<UAnimMontage>>& Back,
		const TArray<TObjectPtr<UAnimMontage>>& Left, const TArray<TObjectPtr<UAnimMontage>>& Right)
	{
		const TArray<TObjectPtr<UAnimMontage>>* List = &Front;
		switch (Direction)
		{
		case ESLHitDirection::Front: List = &Front; break;
		case ESLHitDirection::Back:  List = &Back;  break;
		case ESLHitDirection::Left:  List = &Left;  break;
		case ESLHitDirection::Right: List = &Right; break;
		}

		if (List->IsEmpty()) return nullptr;
		return (*List)[FMath::RandRange(0, List->Num() - 1)];
	}
}
