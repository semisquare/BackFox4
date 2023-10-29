// Fill out your copyright notice in the Description page of Project Settings.


#include "JAIController.h"

void AJAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//if (PlayerPawn)
	//{
	//	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
	//}
}
