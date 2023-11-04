// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "JCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartAttack, AActor*, InstigatorActor);

class USpringArmComponent;
class UCameraComponent;
class UJAttributeComponent;
class UWidgetComponent;
class UInputMappingContext;
class UInputAction;
class UJAnimInstance;
class UNiagaraSystem;

UCLASS()
class BACKFOX4_API AJCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;
	float IdleArmLength;
	float RunArmLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UJAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* CharacterUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* RightWeapon;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* AccelerateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* AttackAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		bool bIsMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		bool bIsRun;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		bool bIsPressedJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		bool bIsPressedAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		float RunSpeed;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShakeBase> AccelerateCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShakeBase> AttackCameraShake;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = "true"))
		UNiagaraSystem* WeaponEffect;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UUserWidget> FloatTextWidget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
		USoundBase* WeaponES;


	FTimerHandle AttackBiasTimer;
	FTimerHandle AttackShakeTimer;

	TArray<FHitResult> FocusHitResults;
	TArray<AActor*> ActorsToIgnoreFocus;

	UJAnimInstance* AnimClass;


	APlayerController* PlayerController;

public:
	// Sets default values for this character's properties
	AJCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	void StartAccelerate(const FInputActionValue& Value);
	void StopAccelerate(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void AttackTimeElapsed();

	UFUNCTION(BlueprintCallable, Category = Input, meta = (AllowPrivateAccess = "true"))
		void AttackMove();

	UFUNCTION(BlueprintCallable, Category = Input, meta = (AllowPrivateAccess = "true"))
		void SetAttackDirection();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable)
		FOnStartAttack OnStartAttack;

	void StartAttackShake(USkeletalMeshComponent* MeshComp, class AJAICharacter* HitEnemy, FVector NiagaraLocation, FRotator NiagaraRotator);
	void StopAttackShake();

	UJAttributeComponent* GetAttributeComponent();
};
