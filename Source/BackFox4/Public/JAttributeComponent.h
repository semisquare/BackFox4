// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChanged, AActor*, InstigatorActor, UJAttributeComponent*, OwingComp, float, NewHealth, float, Delta, bool, bIsNoHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInDeath, AActor*, InstigatorActor, UJAttributeComponent*, OwingComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnStaminaChanged, AActor*, InstigatorActor, UJAttributeComponent*, OwingComp, float, NewStamina, float, Delta, bool, bIsNoStamina);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BACKFOX4_API UJAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJAttributeComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		bool bIsDeath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float StaminaMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		bool bIsExhasuted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float Damage;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		bool ApplyHealthChange(float Delta);

	UFUNCTION(BlueprintCallable)
		bool IsDead();

	UPROPERTY(BlueprintAssignable)
		FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
		FInDeath InDeath;

	UFUNCTION(BlueprintCallable)
		bool ApplyStaminaChange(float Delta);

	UFUNCTION(BlueprintCallable)
		bool IsRecoverStamina();

	UFUNCTION(BlueprintCallable)
		bool IsExhausted();

	UPROPERTY(BlueprintAssignable)
		FOnStaminaChanged OnStaminaChanged;

	UFUNCTION(BlueprintCallable)
		float GetDamage();
};
