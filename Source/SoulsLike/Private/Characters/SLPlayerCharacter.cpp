// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/SLPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "KismetAnimationLibrary.h"
#include "Camera/CameraComponent.h"
#include "Combat/SLLockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ASLPlayerCharacter::ASLPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->MaxAcceleration = 1500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1800.f;
	GetCharacterMovement()->MaxWalkSpeed = 490.f;
	
	LockComponent = CreateDefaultSubobject<USLLockOnComponent>(TEXT("LockOnComponent"));
	
	
}

bool ASLPlayerCharacter::IsStrafing() const
{
	return LockComponent && LockComponent->IsLockedOn();
}

// Called when the game starts or when spawned
void ASLPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	LockComponent->OnLockOnChanged.AddUObject(this, &ASLPlayerCharacter::HandleLockOnChanged);
}

// Called every frame
void ASLPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsStrafing())
	{
		GetCharacterMovement()->MaxWalkSpeed = 490.f;
		return;
	}

	const float Direction = FMath::Abs(
		UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation()));

	const float Speed = Direction <= 90.f
		? FMath::GetMappedRangeValueClamped(FVector2f(0.f, 90.f), FVector2f(490.f, 320.f), Direction)
		: FMath::GetMappedRangeValueClamped(FVector2f(90.f, 180.f), FVector2f(320.f, 290.f), Direction);

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

// Called to bind functionality to input
void ASLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (PlayerInputComponent)
	{
		EIC->BindAction(MoveAction , ETriggerEvent::Triggered, this, &ASLPlayerCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASLPlayerCharacter::Look);
		EIC->BindAction(LockOnAction, ETriggerEvent::Started, this, &ASLPlayerCharacter::LockOn);
	}
}

void ASLPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	FRotator YawRotation(0.f, GetControlRotation().Yaw,0.f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(Forward, AxisValue.Y);
	AddMovementInput(Right, AxisValue.X);
}

void ASLPlayerCharacter::Look(const FInputActionValue& Value)
{
	// Kilitliyken kamerayı lock-on component yönetir
	if (LockComponent->IsLockedOn()) return;

	FVector2D AxisValue = Value.Get<FVector2D>();
	AddControllerYawInput(AxisValue.X);
	AddControllerPitchInput(AxisValue.Y);
}

void ASLPlayerCharacter::LockOn(const FInputActionValue& Value)
{
	LockComponent->ToggleLockOn();
}

void ASLPlayerCharacter::HandleLockOnChanged(AActor* NewTarget)
{
	const bool bLocked = NewTarget != nullptr;

	// Serbest: gittiği yöne döner. Kilitli: kameraya, yani hedefe dönük kalır.
	GetCharacterMovement()->bOrientRotationToMovement = !bLocked;
	GetCharacterMovement()->bUseControllerDesiredRotation = bLocked;
}

