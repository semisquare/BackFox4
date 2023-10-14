// Fill out your copyright notice in the Description page of Project Settings.


#include "JAnimNotifyState_AttackValid.h"
#include "JCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"


void UJAnimNotifyState_AttackValid::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!Player) return;

	UKismetSystemLibrary::LineTraceMulti(Player->GetWorld(), Weapon->GetSocketLocation("RW_Begin"), Weapon->GetSocketLocation("RW_End"),
		TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

	for (int i = 0; i < HitResults.Num(); i++)
	{
		//获取本次射线击中的敌人
		AActor* HitEnemy = HitResults[i].GetActor();
		//查询数组中是否有本次击中的Actor，如果没有则加入数组，防止一次通知内多次击中的情况
		if (HitEnemy && !HitEnemies.Contains(HitEnemy) && HitEnemy->ActorHasTag("Enemy"))
		{
			GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Red, HitEnemy->GetName());
			HitEnemies.Add(HitEnemy);

			Player->StartAttackShake();

			//if (HitEnemy->InteractParticle) { //播放敌人受伤粒子特效
			//	UGameplayStatics::SpawnEmitterAtLocation(HitEnemy, HitEnemy->InteractParticle, HitResults[i].Location);
			//}
			//if (HitEnemy->ReactSound) { // 播放敌人受伤音乐
			//	UGameplayStatics::PlaySound2D(HitEnemy, HitEnemy->ReactSound);
			//}
		}
	}
}

void UJAnimNotifyState_AttackValid::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Player = Cast<AJCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Weapon = Player->GetMesh();
		ActorsToIgnore = { MeshComp->GetOwner() };
	}
}

void UJAnimNotifyState_AttackValid::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitEnemies.Empty();
}
