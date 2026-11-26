// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Core/SLPlayerController.h"

#include "EnhancedInputSubsystems.h"

void ASLPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if (Subsystem && DefaultMappingContext)
	{
		Subsystem->AddMappingContext(DefaultMappingContext,0);
	}
}
