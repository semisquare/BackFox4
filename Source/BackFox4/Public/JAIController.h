// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "JAIController.generated.h"

/**
 * 
 */
UCLASS()
class BACKFOX4_API AJAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		UBehaviorTree* BehaviorTree;

protected:
	virtual void BeginPlay() override;
	
};
