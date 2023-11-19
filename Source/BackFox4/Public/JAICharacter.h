// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JAICharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttacked, AActor*, InstigatorActor, float, Damage);

class UJAttributeComponent;
class UPawnSensingComponent;
class UWidgetComponent;
class UBlackboardComponent;
class UNiagaraSystem;

UCLASS()
class BACKFOX4_API AJAICharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UJAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* CharacterUI;


	FTimerHandle AttackedTimer;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bIsAttacked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bIsAlive;

	UBlackboardComponent* BBComp;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = "true"))
		UNiagaraSystem* DeathEffect;

	class AJCharacter* JPlayer;

public:
	// Sets default values for this character's properties
	AJAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnSeePawn(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category = Input, meta = (AllowPrivateAccess = "true"))
		void SetDeath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable)
		FOnAttacked OnAttacked;

	void Attacked(class AJCharacter* Player, FName HittedBone, FVector HitPoint, FVector HitImpulse);
	void AttackedEnd(FName HittedBone, FVector InitialLocation, AJCharacter* Player);
};
