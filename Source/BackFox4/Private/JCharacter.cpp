
// Fill out your copyright notice in the Description page of Project Settings.


#include "JCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AJCharacter::AJCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetRelativeLocation(FVector(0, 0, 70));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraLagSpeed = 8.0f;
	SpringArmComponent->CameraRotationLagSpeed = 9.0f;
	
	IdleArmLength = 420.0f;
	RunArmLength = 500.0f;
	SpringArmComponent->TargetArmLength = IdleArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	

	bIsPressedJump = false;
	bIsRun = false;
	WalkSpeed = 350.0f;
	RunSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void AJCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AJCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, RunSpeed, DeltaTime, 7.0f);
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, RunArmLength, DeltaTime, 7.0f);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, WalkSpeed, DeltaTime, 4.0f);
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, IdleArmLength, DeltaTime, 4.0f);
	}
}

// Called to bind functionality to input
void AJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJCharacter::Move);

		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &AJCharacter::StartAccelerate);
		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Completed, this, &AJCharacter::StopAccelerate);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJCharacter::Look);

	}
}

void AJCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AJCharacter::StartAccelerate(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		bIsRun = true;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(AccelerateCameraShake);
		}
		
	}
}

void AJCharacter::StopAccelerate(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		bIsRun = false;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(AccelerateCameraShake);
		}
	}
}

void AJCharacter::StartJump(const FInputActionValue& Value)
{
	if (!bIsPressedJump)
	{
		bIsPressedJump = true;

		GetCharacterMovement()->Velocity = FVector(0);
		//GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}
}

void AJCharacter::Jumpping()
{
	//GetCharacterMovement()->MaxWalkSpeed = StartMaxWalkSpeed;

	//const FRotator Rotation = GetActorRotation();
	//const FRotator YawRotation(0, Rotation.Yaw, 0);

	//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	//GetCharacterMovement()->Velocity = ForwardDirection * 1100.0f;
}

void AJCharacter::Land()
{
	GetCharacterMovement()->Velocity = FVector(0);
	//GetCharacterMovement()->MaxWalkSpeed = 0.0f;
}

void AJCharacter::StopJump()
{
	if (bIsPressedJump)
	{
		bIsPressedJump = false;

		GetCharacterMovement()->Velocity = FVector(0);
		//GetCharacterMovement()->MaxWalkSpeed = StartMaxWalkSpeed;
	}
}

void AJCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
