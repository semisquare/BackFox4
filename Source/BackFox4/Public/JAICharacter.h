// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JAICharacter.generated.h"

class UJAttributeComponent;

UCLASS()
class BACKFOX4_API AJAICharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		UJAttributeComponent* AttributeComponent;


	FTimerHandle AttackedTimer;

public:
	// Sets default values for this character's properties
	AJAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attacked(class AJCharacter* Player, FName HittedBone, FVector HitPoint, FVector HitImpulse);
	void AttackedEnd(FName HittedBone, FVector InitialLocation);
};
