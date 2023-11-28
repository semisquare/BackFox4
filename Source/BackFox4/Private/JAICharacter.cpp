// Fill out your copyright notice in the Description page of Project Settings.


#include "JAICharacter.h"
#include "JCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "JAttributeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BrainComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "JAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AJAICharacter::AJAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	AttributeComponent = CreateDefaultSubobject<UJAttributeComponent>("AttributeComponent");

	CharacterUI = CreateDefaultSubobject<UWidgetComponent>("CharacterUI");
	CharacterUI->SetupAttachment(RootComponent);
	
	bIsAttacked = false;
	bIsAlive = true;
	FinishAllAttack();
	

	JPlayer = nullptr;

	
}

// Called when the game starts or when spawned
void AJAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AJAICharacter::OnSeePawn);

	AnimClass = Cast<UJAnimInstance>(GetMesh()->AnimScriptInstance);
}

void AJAICharacter::OnSeePawn(APawn* Pawn)
{
	if (Pawn->ActorHasTag("Player"))
	{
		JPlayer = Cast<AJCharacter>(Pawn);

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			BBComp = AIController->GetBlackboardComponent();
			BBComp->SetValueAsObject("TargetActor", Pawn);
		}
	}
}

void AJAICharacter::SetDeath()
{
	if (JPlayer)
	{
		JPlayer->LockFunc();
	}
	

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathEffect, GetMesh()->GetBoneLocation("Hips"), FRotator(0), GetActorScale());

	Destroy();
}

// Called every frame
void AJAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (JPlayer && !AnimClass->bIsInAttack)
	{
		//GEngine->AddOnScreenDebugMessage(0, 1.1f, FColor::Red, AnimClass->bIsInAttack ? TEXT("True") : TEXT("False"));
		FVector DirectionTo = JPlayer->GetActorLocation() - GetActorLocation();
		DirectionTo.Normalize();
		FMatrix rotationMatrix = FRotationMatrix::MakeFromXZ(DirectionTo, GetOwner()->GetActorUpVector());
		SetActorRotation(FRotator(GetActorRotation().Pitch, rotationMatrix.Rotator().Yaw, GetActorRotation().Roll));
		GetController()->SetControlRotation(GetActorRotation());
	}

	//if (JPlayer && BBComp)
	//{
	//	float DistanceTo = FVector::Dist2D(GetActorLocation(), JPlayer->GetActorLocation());
	//	if (DistanceTo < 100.0f)
	//	{
	//		BBComp->SetValueAsBool("CanAttack", true);
	//		
	//	}
	//	else
	//	{
	//		BBComp->SetValueAsBool("CanAttack", false);
	//	}
	//}
}

// Called to bind functionality to input
void AJAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJAICharacter::Attacked(AJCharacter* Player, FName HittedBone, FVector HitPoint, FVector HitImpulse)
{
	if (HittedBone == "None" || HittedBone == "pelvis" || HittedBone == "Root" || HittedBone == "Hips") return;

	if(!JPlayer){ JPlayer = Player; }

	bIsAttacked = true;

	FVector InitialLocation = GetMesh()->GetRelativeLocation();

	FVector DirectionTo = GetMesh()->GetBoneLocation(HittedBone) - Player->GetActorLocation();
	DirectionTo = FVector(DirectionTo.X, DirectionTo.Y, 0);
	DirectionTo.Normalize();
	ApplyWorldOffset(DirectionTo * 106.f, true);

	UJAttributeComponent* PlayerAttributeComp = Player->GetComponentByClass<UJAttributeComponent>();
	float PlayerDamage = PlayerAttributeComp->GetDamage();
	float ActualDamage = int(FMath::Max(FMath::RandRange(PlayerDamage - 3, PlayerDamage + 3), 1));
	AttributeComponent->ApplyHealthChange(-ActualDamage);

	//GetMesh()->SetAllBodiesBelowSimulatePhysics(HittedBone, true);
	//GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HittedBone, 0.3f);
	//GetMesh()->AddImpulseAtLocation(HitImpulse * 22.f, HitPoint, HittedBone);

	OnAttacked.Broadcast(nullptr, ActualDamage);

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AJAICharacter::AttackedEnd, HittedBone, InitialLocation, Player);
	GetWorldTimerManager().SetTimer(AttackedTimer, TimerDelegate, 0.1f, false);
}

void AJAICharacter::AttackedEnd(FName HittedBone, FVector InitialLocation, AJCharacter* Player)
{
	bIsAttacked = false;

	//GetMesh()->SetAllBodiesSimulatePhysics(false);
	//GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HittedBone, 0);
	//GetMesh()->SetRelativeLocation(InitialLocation);

	//GetMesh()->SetRelativeRotation(FRotator(0, 0, GetMesh()->GetRelativeRotation().Roll));

	if (AttributeComponent->IsDead())
	{
		bIsAlive = false;

		GetMesh()->SetCollisionProfileName("NoCollision");

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			BBComp = AIController->GetBlackboardComponent();

			AIController->GetBrainComponent()->StopLogic("Dead");
		}
		
		
	}
}

void AJAICharacter::AttackMove()
{
	GetCharacterMovement()->Velocity = GetActorForwardVector() * 620.0f;
}

void AJAICharacter::FinishAllAttack()
{
	bIsAttack1 = false;
	bIsAttack2 = false;
	bIsAttack3 = false;
	bIsAttack4 = false;
	bIsAttack5 = false;
}

float AJAICharacter::SetAttack(int32 AttackIndex)
{
	float AttackTime = 0.f;

	switch (AttackIndex) {
		case 1:
			bIsAttack1 = true;
			AttackTime = 4.f;
			break;
		case 2:
			bIsAttack2 = true;
			AttackTime = 5.f;
			break;
		case 3:
			bIsAttack3 = true;
			AttackTime = 1.f;
			break;
		case 4:
			bIsAttack4 = true;
			AttackTime = 1.f;
			break;
		case 5:
			bIsAttack5 = true;
			AttackTime = 2.f;
			break;
	}

	return AttackTime;
}



