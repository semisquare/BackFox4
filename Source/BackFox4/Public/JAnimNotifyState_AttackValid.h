// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "JAnimNotifyState_AttackValid.generated.h"

/**
 * 
 */
UCLASS()
class BACKFOX4_API UJAnimNotifyState_AttackValid : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	TArray<FHitResult> HitResults;

	TArray<AActor*> HitEnemies;

	class AJCharacter* Player;

	class USkeletalMeshComponent* Weapon;

	TArray<AActor*>ActorsToIgnore;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)override;
	
};
