// Fill out your copyright notice in the Description page of Project Settings.


#include "JAICharacter.h"
#include "JCharacter.h"
#include "JAttributeComponent.h"

// Sets default values
AJAICharacter::AJAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UJAttributeComponent>("AttributeComponent");

}

// Called when the game starts or when spawned
void AJAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJAICharacter::Attacked(AJCharacter* Player, FName HittedBone, FVector HitPoint, FVector HitImpulse)
{
	if (HittedBone == "None" || HittedBone == "pelvis" || HittedBone == "Root" || HittedBone == "Hips") return;

	FVector InitialLocation = GetMesh()->GetRelativeLocation();

	FVector DirectionTo = GetMesh()->GetBoneLocation(HittedBone) - Player->GetActorLocation();
	DirectionTo = FVector(DirectionTo.X, DirectionTo.Y, 0);
	DirectionTo.Normalize();
	//ApplyWorldOffset(DirectionTo * 22.f, true);

	GetMesh()->SetAllBodiesBelowSimulatePhysics(HittedBone, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HittedBone, 0.3f);
	GetMesh()->AddImpulseAtLocation(HitImpulse * 22.f, HitPoint, HittedBone);

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AJAICharacter::AttackedEnd, HittedBone, InitialLocation);
	GetWorldTimerManager().SetTimer(AttackedTimer, TimerDelegate, 0.1f, false);
}

void AJAICharacter::AttackedEnd(FName HittedBone, FVector InitialLocation)
{
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HittedBone, 0);
	GetMesh()->SetRelativeLocation(InitialLocation);
	//GetMesh()->SetRelativeRotation(FRotator(0, 0, GetMesh()->GetRelativeRotation().Roll));
}



