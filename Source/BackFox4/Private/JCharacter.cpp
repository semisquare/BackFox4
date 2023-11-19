
// Fill out your copyright notice in the Description page of Project Settings.


#include "JCharacter.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "JAttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "JAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "JAICharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"

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
	
	IdleArmLength = 380.0f;
	RunArmLength = 480.0f;
	SpringArmComponent->TargetArmLength = IdleArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	AttributeComponent = CreateDefaultSubobject<UJAttributeComponent>("AttributeComponent");

	CharacterUI = CreateDefaultSubobject<UWidgetComponent>("CharacterUI");
	CharacterUI->SetupAttachment(RootComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	

	bIsMove = false;
	bIsRun = false;
	bIsDodge = false;
	bIsPressedJump = false;
	bIsPressedLock = false;
	bIsLock = false;
	bIsPressedAttack = false;
	bIsPressedAttackLong = false;

	LockEnemy = nullptr;
	
	WalkSpeed = 370.0f;
	RunSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
	

// Called when the game starts or when spawned
void AJCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetController());
	//Add Input Mapping Context
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	AnimClass = Cast<UJAnimInstance>(GetMesh()->AnimScriptInstance);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, UKismetStringLibrary::Conv_BoolToString(AnimClass->bIsInAttack));
}

// Called every frame
void AJCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRun && !AttributeComponent->IsExhausted())
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, RunSpeed, DeltaTime, 7.0f);

		AttributeComponent->ApplyStaminaChange(-26.0f * DeltaTime);

		if (GetCharacterMovement()->Velocity.Length())
		{
			SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, RunArmLength, DeltaTime, 7.0f);
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, WalkSpeed, DeltaTime, 4.0f);

		if (AttributeComponent->IsRecoverStamina())
		{
			AttributeComponent->ApplyStaminaChange(38.0f * DeltaTime);
		}

		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, IdleArmLength, DeltaTime, 4.0f);
	}

	if (bIsLock && LockEnemy != nullptr)
	{
		FVector DirectionTo = LockEnemy->GetActorLocation() - GetActorLocation();
		DirectionTo.Normalize();
		FMatrix rotationMatrix = FRotationMatrix::MakeFromXZ(DirectionTo, GetOwner()->GetActorUpVector());
		if (!bIsDodge) SetActorRotation(FRotator(GetActorRotation().Pitch, rotationMatrix.Rotator().Yaw, GetActorRotation().Roll));
		GetController()->SetControlRotation(FRotator(FMath::Clamp(rotationMatrix.Rotator().Pitch, -50, 10), rotationMatrix.Rotator().Yaw, GetControlRotation().Roll));

		//float Angle = FVector2D::CrossProduct(FVector2D(SpringArmComponent->GetForwardVector()), FVector2D(LockEnemy->GetActorLocation() - GetActorLocation()));
		//AddControllerYawInput(Angle);
	}
}

// Called to bind functionality to input
void AJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJCharacter::Move);
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AJCharacter::Move);

		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Canceled, this, &AJCharacter::Dodge);
		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &AJCharacter::StartAccelerate);
		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Completed, this, &AJCharacter::StopAccelerate);

		//EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &AJCharacter::Dodge);
		//EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Completed, this, &AJCharacter::StopAccelerate);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AJCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJCharacter::Look);

		EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Triggered, this, &AJCharacter::Lock);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &AJCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AJCharacter::AttackLong);
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AJCharacter::Attack);
	}
}

void AJCharacter::Move(const FInputActionValue& Value)
{
	if (!AnimClass->bIsInAttack)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();
		//if (bIsDodge) { MovementVector = FVector2D(GetActorForwardVector());}
		//else { MovementVector = Value.Get<FVector2D>(); }
		//MovementVector.Normalize();
		bIsMove = MovementVector.Length() > 0.0f;

		if (Controller != nullptr)
		{
			// find out which way is forward
			FRotator Rotation = Controller->GetControlRotation();;
			//if(bIsLock) { Rotation = GetActorRotation(); }
			//else { Rotation = Controller->GetControlRotation(); }

			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);

			AddControllerYawInput(MovementVector.X * 0.4f);
		}
	}
	else
	{
		bIsMove = false;
	}
}

void AJCharacter::StartAccelerate(const FInputActionValue& Value)
{
	if (Controller != nullptr && bIsMove)
	{
		bIsRun = true;

		if (PlayerController)
		{
			if (AttributeComponent->IsExhausted())
			{
				PlayerController->ClientStopCameraShake(AccelerateCameraShake);
			}
			else
			{
				PlayerController->ClientStartCameraShake(AccelerateCameraShake);
			}
		}
		
	}
	else
	{
		bIsRun = false;
	}
}

void AJCharacter::StopAccelerate(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		if (bIsRun)
		{
			bIsRun = false;

			if (PlayerController)
			{
				PlayerController->ClientStopCameraShake(AccelerateCameraShake);
			}
		}
		if (bIsDodge)
		{
			bIsDodge = false;
		}

	}
}

void AJCharacter::Dodge(const FInputActionValue& Value)
{
	if (GetMovementComponent()->IsFalling() || AnimClass->bIsInAttack) return;

	if (Controller != nullptr)
	{
		if (!bIsDodge && !AttributeComponent->IsExhausted())
		{
			bIsDodge = true;

			FVector Direction;
			if (GetVelocity().Length() > 0) { Direction = GetVelocity(); }
			else { Direction = GetActorForwardVector(); }
			Direction.Normalize();
			GetCharacterMovement()->Velocity = Direction * 1980.0f;
			AttributeComponent->ApplyStaminaChange(-50.0f);

			if (PlayerController)
			{
				PlayerController->ClientStartCameraShake(AccelerateCameraShake);
			}
			SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, RunArmLength, 0.03f, 7.0f);
		}
	}
}

void AJCharacter::DodgeEnd()
{
	bIsDodge = false;

	PlayerController->ClientStopCameraShake(AccelerateCameraShake);
	SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, IdleArmLength, 0.01f, 4.0f);

	//GetWorldTimerManager().SetTimer(DodgeTimer, this, &AJCharacter::DodgeTimeElapsed, 1.0f);
}

void AJCharacter::DodgeTimeElapsed()
{

}

void AJCharacter::Jump()
{
	if (!bIsDodge)
	{
		Super::Jump();

		GetCharacterMovement()->Velocity += GetActorForwardVector() * 10.0f;
	}
}

void AJCharacter::Land()
{
	GetCharacterMovement()->Velocity = FVector(0);
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

void AJCharacter::Lock(const FInputActionValue& Value)
{
	if (GetMovementComponent()->IsFalling() || bIsDodge) return;

	LockFunc();
}

void AJCharacter::LockTimeElapsed()
{
	bIsPressedLock = false;
}

void AJCharacter::LockFunc()
{
	bIsPressedLock = true;

	if (bIsLock)
	{
		bIsLock = false;

		//bUseControllerRotationYaw = false;
		//SpringArmComponent->bUsePawnControlRotation = true;

		GetController()->SetControlRotation(GetActorRotation());

		LockEnemy = nullptr;
	}
	else
	{
		FocusHitResults.Empty();

		UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 2280.f, ETraceTypeQuery::TraceTypeQuery3,
			false, ActorsToIgnoreFocus, EDrawDebugTrace::None, FocusHitResults, true);

		if (FocusHitResults.Num() > 0)
		{
			float DistanceTo = 9999999.0f;
			bool bIsRotate = false;
			FVector2D CameraDirection = FVector2D(UKismetMathLibrary::GetForwardVector(GetControlRotation()));

			for (int i = 0; i < FocusHitResults.Num(); i++)
			{
				AActor* FocusOne = FocusHitResults[i].GetActor();

				FVector2D TempDirectionTo = FVector2D(FocusOne->GetActorLocation() - CameraComponent->GetComponentLocation());
				TempDirectionTo.Normalize();

				float Angle = FVector2D::CrossProduct(CameraDirection, TempDirectionTo) * 90;

				if (FocusOne && FocusOne->ActorHasTag("Enemy") && FMath::Abs(Angle) < 60.0f)
				{
					float TempDistanceTo = FVector::Dist2D(FocusOne->GetActorLocation(), GetActorLocation());
					if (DistanceTo > TempDistanceTo) {
						DistanceTo = TempDistanceTo;

						LockEnemy = FocusOne;
					}
				}
			}
		}

		if (LockEnemy != nullptr)
		{
			bIsLock = true;

			//bUseControllerRotationYaw = true;
			//SpringArmComponent->bUsePawnControlRotation = false;
		}
	}

	OnLock.Broadcast(LockEnemy, bIsLock);

	GetWorldTimerManager().SetTimer(LockTimer, this, &AJCharacter::LockTimeElapsed, 0.1f);
}

void AJCharacter::Attack(const FInputActionValue& Value)
{
	GetWorldTimerManager().PauseTimer(AttackBiasTimer);
	
	if (!bIsPressedAttack && !bIsDodge)
	{
		bIsPressedAttack = true;

		GetCharacterMovement()->Velocity = GetActorForwardVector() * 0.0f;
	}

	GetWorldTimerManager().SetTimer(AttackBiasTimer, this, &AJCharacter::AttackTimeElapsed, 0.4f);
}

void AJCharacter::AttackTimeElapsed()
{
	bIsPressedAttack = false;
	bIsPressedAttackLong = false;
}

void AJCharacter::AttackLong(const FInputActionValue& Value)
{
	GetWorldTimerManager().PauseTimer(AttackBiasTimer);

	if (!bIsPressedAttackLong && !bIsDodge)
	{
		bIsPressedAttackLong = true;

		GetCharacterMovement()->Velocity = GetActorForwardVector() * 0.0f;
	}

	GetWorldTimerManager().SetTimer(AttackBiasTimer, this, &AJCharacter::AttackTimeElapsed, 0.4f);
}

void AJCharacter::AttackMove()
{
	//const FRotator Rotation = GetActorRotation();
	//const FRotator YawRotation(0, Rotation.Yaw, 0);
	//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	//if (bIsPressedAttack) { GetCharacterMovement()->Velocity = GetActorForwardVector() * 420.0f; }
	if (bIsPressedAttackLong) { GetCharacterMovement()->Velocity = GetActorForwardVector() * 820.0f; }
	else { GetCharacterMovement()->Velocity = GetActorForwardVector() * 420.0f; }
	
}

void AJCharacter::SetAttackDirection()
{
	FocusHitResults.Empty();

	UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 480.f, ETraceTypeQuery::TraceTypeQuery3,
		false, ActorsToIgnoreFocus, EDrawDebugTrace::None, FocusHitResults, true);

	if (FocusHitResults.Num() <= 0) return;

	float DistanceTo = 9999999.0f;
	FVector DirectionTo = FVector(0);
	bool bIsRotate = false;

	for (int i = 0; i < FocusHitResults.Num(); i++)
	{
		AActor* FocusOne = FocusHitResults[i].GetActor();
		if (FocusOne && FocusOne->ActorHasTag("Enemy"))
		{
			float TempDistanceTo = FVector::Dist2D(FocusOne->GetActorLocation(), GetActorLocation());
			if (DistanceTo > TempDistanceTo) {
				DistanceTo = TempDistanceTo;

				DirectionTo = FocusOne->GetActorLocation() - GetActorLocation();
				DirectionTo = FVector(DirectionTo.X, DirectionTo.Y, 0);
				DirectionTo.Normalize();

				//GEngine->AddOnScreenDebugMessage(0, 1.1f, FColor::Red, FocusOne->GetActorLocation().ToString());

				bIsRotate = true;
			}
		}
		
	}

	if (bIsRotate)
	{
		//GetController()->SetControlRotation(DirectionTo.Rotation());
		SetActorRotation(DirectionTo.Rotation());
	}
}

void AJCharacter::StartAttackShake(USkeletalMeshComponent* MeshComp, AJAICharacter* HitEnemy, FVector NiagaraLocation, FRotator NiagaraRotator)
{
	UGameplayStatics::PlaySound2D(this, WeaponES, 1.0f, FMath::RandRange(0.8f, 1.2f));

	PlayerController->ClientStartCameraShake(AttackCameraShake);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp, WeaponEffect, NiagaraLocation, NiagaraRotator);
	//if (FloatTextWidget)
	//{
	//	UUserWidget* FloatTextWidgetRef = CreateWidget<UUserWidget>(GetWorld(), FloatTextWidget);
	//	if (FloatTextWidgetRef)
	//	{
	//		FVector2D ScreenPosition = FVector2D(0);
	//		UGameplayStatics::ProjectWorldToScreen(Cast<APlayerController>(GetController()), HitEnemy->GetActorLocation(), ScreenPosition);

	//		FloatTextWidgetRef->AddToViewport();
	//		FloatTextWidgetRef->SetPositionInViewport(ScreenPosition / UWidgetLayoutLibrary::GetViewportScale(this));
	//	}
	//}

	GetMesh()->GlobalAnimRateScale = 0.1f;

	GetWorldTimerManager().SetTimer(AttackShakeTimer, this, &AJCharacter::StopAttackShake, 0.1f);

	OnStartAttack.Broadcast(MeshComp->GetOwner());
}

void AJCharacter::StopAttackShake()
{
	GetMesh()->GlobalAnimRateScale = 1.f;

	PlayerController->ClientStartCameraShake(AttackCameraShake);
}

UJAttributeComponent* AJCharacter::GetAttributeComponent()
{
	return AttributeComponent;
}
