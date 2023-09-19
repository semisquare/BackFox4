// Fill out your copyright notice in the Description page of Project Settings.


#include "JAnimInstance.h"
#include "JCharacter.h"
#include "Kismet/KismetStringLibrary.h"

void UJAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		AJCharacter* Character = Cast<AJCharacter>(OwningActor);
		if (Character)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("FUCKYOU"));
		}
	}

	bIsInAttack = false;
}

void UJAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}