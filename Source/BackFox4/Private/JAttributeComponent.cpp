// Fill out your copyright notice in the Description page of Project Settings.


#include "JAttributeComponent.h"

// Sets default values for this component's properties
UJAttributeComponent::UJAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	StaminaMax = 100.0f;
	Stamina = StaminaMax;
	bIsExhasuted = false;
}

// Called when the game starts
void UJAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UJAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UJAttributeComponent::ApplyStaminaChange(float Delta)
{
	float TempStamina = Stamina;

	Stamina = FMath::Clamp(Stamina + Delta, 0.0f, StaminaMax);

	float ActualDelta = Stamina - TempStamina;

	if (ActualDelta != 0)
	{
		if (Stamina <= 0.0f)
		{
			bIsExhasuted = true;
		}

		if (bIsExhasuted)
		{
			if (Stamina >= StaminaMax)
			{
				bIsExhasuted = false;
			}
		}
	}

	OnStaminaChanged.Broadcast(nullptr, this, Stamina, ActualDelta, bIsExhasuted);

	return ActualDelta != 0;
}

bool UJAttributeComponent::IsRecoverStamina()
{
	return Stamina < StaminaMax;
}

bool UJAttributeComponent::IsExhausted()
{
	return bIsExhasuted;
}

