// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JAttributeComponent.generated.h"

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
		float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		float StaminaMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
		bool bIsExhasuted;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		bool ApplyStaminaChange(float Delta);

	UFUNCTION(BlueprintCallable)
		bool IsRecoverStamina();

	UFUNCTION(BlueprintCallable)
		bool IsExhausted();

	UPROPERTY(BlueprintAssignable)
		FOnStaminaChanged OnStaminaChanged;
};
