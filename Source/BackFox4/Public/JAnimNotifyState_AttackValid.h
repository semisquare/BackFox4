// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "JAnimNotifyState_AttackValid.generated.h"

class AJCharacter;
class AJAICharacter;

UENUM(BlueprintType)
enum class EAttackInstigator : uint8
{
	EI_Player       UMETA(DisplayName = "Player"),
	EI_Enemy        UMETA(DisplayName = "Enemy"),
};

UCLASS()
class BACKFOX4_API UJAnimNotifyState_AttackValid : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
		EAttackInstigator AttackInstigator = EAttackInstigator::EI_Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
		bool bIsLeftS = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
		bool bIsRightS = false;

	TArray<FHitResult> HitResults;

	TArray<AJCharacter*> HitPlayers;
	TArray<AJAICharacter*> HitEnemies;

	AJCharacter* AttackPlayer;
	AJAICharacter* AttackEnemy;

	class USkeletalMeshComponent* Weapon;

	TArray<AActor*>ActorsToIgnore;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)override;
	
};


