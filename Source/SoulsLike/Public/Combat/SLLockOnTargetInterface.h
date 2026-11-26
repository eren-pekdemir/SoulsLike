// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLLockOnTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USLLockOnTargetInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULSLIKE_API ISLLockOnTargetInterface
{
	GENERATED_BODY()
public:
	
	virtual FVector GetLockOnLocation() const = 0;
	virtual bool IsLockOnAllowed() const = 0;
};
