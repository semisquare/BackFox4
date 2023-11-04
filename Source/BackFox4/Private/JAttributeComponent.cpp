// Fill out your copyright notice in the Description page of Project Settings.


#include "JAttributeComponent.h"

// Sets default values for this component's properties
UJAttributeComponent::UJAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	HealthMax = 100.0f;
	Health = HealthMax;
	bIsDeath = false;

	StaminaMax = 100.0f;
	Stamina = StaminaMax;
	bIsExhasuted = false;

	Damage = 10.0f;
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

bool UJAttributeComponent::ApplyHealthChange(float Delta)
{
	float TempHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - TempHealth;

	if (ActualDelta != 0)
	{
		if (Health <= 0.0f)
		{
			bIsDeath = true;

			InDeath.Broadcast(nullptr, this);
		}

		if (bIsDeath)
		{
			if (Health >= HealthMax)
			{
				bIsDeath = false;
			}
		}
	}

	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta, bIsDeath);

	return ActualDelta != 0;
}

bool UJAttributeComponent::IsDead()
{
	return bIsDeath;
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

float UJAttributeComponent::GetDamage()
{
	return Damage;
}

