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
}

// Called when the game starts or when spawned
void AJAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AJAICharacter::OnSeePawn);
}

void AJAICharacter::OnSeePawn(APawn* Pawn)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		BBComp = AIController->GetBlackboardComponent();
		BBComp->SetValueAsObject("TargetActor", Pawn);
	}
}

void AJAICharacter::SetDeath()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathEffect, GetMesh()->GetBoneLocation("Hips"), FRotator(0), GetActorScale());

	Destroy();
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

	bIsAttacked = true;

	FVector InitialLocation = GetMesh()->GetRelativeLocation();

	FVector DirectionTo = GetMesh()->GetBoneLocation(HittedBone) - Player->GetActorLocation();
	DirectionTo = FVector(DirectionTo.X, DirectionTo.Y, 0);
	DirectionTo.Normalize();
	ApplyWorldOffset(DirectionTo * 36.f, true);

	UJAttributeComponent* PlayerAttributeComp = Player->GetComponentByClass<UJAttributeComponent>();
	float PlayerDamage = PlayerAttributeComp->GetDamage();
	float ActualDamage = int(FMath::Max(FMath::RandRange(PlayerDamage - 3, PlayerDamage + 3), 1));
	AttributeComponent->ApplyHealthChange(-ActualDamage);

	//GetMesh()->SetAllBodiesBelowSimulatePhysics(HittedBone, true);
	//GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(HittedBone, 0.3f);
	//GetMesh()->AddImpulseAtLocation(HitImpulse * 22.f, HitPoint, HittedBone);

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AJAICharacter::AttackedEnd, HittedBone, InitialLocation);
	GetWorldTimerManager().SetTimer(AttackedTimer, TimerDelegate, 0.1f, false);

	OnAttacked.Broadcast(nullptr, ActualDamage);
}

void AJAICharacter::AttackedEnd(FName HittedBone, FVector InitialLocation)
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



