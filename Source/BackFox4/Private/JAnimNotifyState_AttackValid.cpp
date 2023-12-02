// Fill out your copyright notice in the Description page of Project Settings.


#include "JAnimNotifyState_AttackValid.h"
#include "JCharacter.h"
#include "JAICharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Controller.h"


void UJAnimNotifyState_AttackValid::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (AttackInstigator == EAttackInstigator::EI_Player)
	{
		if (!AttackPlayer) return;

		UKismetSystemLibrary::LineTraceMulti(AttackPlayer->GetWorld(), Weapon->GetSocketLocation("RW_Begin"), Weapon->GetSocketLocation("RW_End"),
			TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

		for (int i = 0; i < HitResults.Num(); i++)
		{
			//��ȡ�������߻��еĵ���
			AJAICharacter* HitEnemy = Cast<AJAICharacter>(HitResults[i].GetActor());
			//��ѯ�������Ƿ��б��λ��е�Actor�����û����������飬��ֹһ��֪ͨ�ڶ�λ��е����
			if (HitEnemy && !HitEnemies.Contains(HitEnemy) && HitEnemy->ActorHasTag("Enemy"))
			{

				HitEnemies.Add(HitEnemy);

				AttackPlayer->StartAttackShake(MeshComp, HitEnemy, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
				HitEnemy->Attacked(AttackPlayer, HitResults[i].BoneName, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal);

				//if (HitEnemy->InteractParticle) { //���ŵ�������������Ч
				//	UGameplayStatics::SpawnEmitterAtLocation(HitEnemy, HitEnemy->InteractParticle, HitResults[i].Location);
				//}
				//if (HitEnemy->ReactSound) { // ���ŵ�����������
				//	UGameplayStatics::PlaySound2D(HitEnemy, HitEnemy->ReactSound);
				//}
			}
		}
	}

	if (AttackInstigator == EAttackInstigator::EI_Enemy)
	{
		if (!AttackEnemy) return;

		UKismetSystemLibrary::BoxTraceMulti(AttackEnemy->GetWorld(), Weapon->GetSocketLocation("RGS_Begin"), Weapon->GetSocketLocation("RGS_End"), FVector(34),
			(Weapon->GetSocketLocation("RGS_Begin") - Weapon->GetSocketLocation("RGS_End")).ToOrientationRotator(), TraceTypeQuery3, false, ActorsToIgnore,
			EDrawDebugTrace::None, HitResults, true);

		for (int i = 0; i < HitResults.Num(); i++)
		{
			AJCharacter* HitPlayer = Cast<AJCharacter>(HitResults[i].GetActor());
			if (HitPlayer && !HitPlayers.Contains(HitPlayer) && HitPlayer->ActorHasTag("Player"))
			{

				HitPlayers.Add(HitPlayer);

				GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Red, "1");
				//AttackEnemy->StartAttackShake(MeshComp, HitPlayer, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
				HitPlayer->Attacked(AttackEnemy, MeshComp, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
			}
		}

		if (bIsLeftS)
		{
			UKismetSystemLibrary::BoxTraceMulti(AttackEnemy->GetWorld(), Weapon->GetSocketLocation("LS_Begin"), Weapon->GetSocketLocation("LS_End"), FVector(28),
				(Weapon->GetSocketLocation("LS_Begin") - Weapon->GetSocketLocation("LS_End")).ToOrientationRotator(), TraceTypeQuery3, false, ActorsToIgnore,
				EDrawDebugTrace::None, HitResults, true);

			for (int i = 0; i < HitResults.Num(); i++)
			{
				AJCharacter* HitPlayer = Cast<AJCharacter>(HitResults[i].GetActor());
				if (HitPlayer && !HitPlayers.Contains(HitPlayer) && HitPlayer->ActorHasTag("Player"))
				{

					HitPlayers.Add(HitPlayer);

					GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Red, "1");
					//AttackEnemy->StartAttackShake(MeshComp, HitPlayer, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
					HitPlayer->Attacked(AttackEnemy, MeshComp, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
				}
			}
		}
		
		if (bIsRightS)
		{
			UKismetSystemLibrary::BoxTraceMulti(AttackEnemy->GetWorld(), Weapon->GetSocketLocation("RS_Begin"), Weapon->GetSocketLocation("RS_End"), FVector(28),
				(Weapon->GetSocketLocation("RS_Begin") - Weapon->GetSocketLocation("RS_End")).ToOrientationRotator(), TraceTypeQuery3, false, ActorsToIgnore,
				EDrawDebugTrace::None, HitResults, true);

			for (int i = 0; i < HitResults.Num(); i++)
			{
				AJCharacter* HitPlayer = Cast<AJCharacter>(HitResults[i].GetActor());
				if (HitPlayer && !HitPlayers.Contains(HitPlayer) && HitPlayer->ActorHasTag("Player"))
				{

					HitPlayers.Add(HitPlayer);

					GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Red, "1");
					//AttackEnemy->StartAttackShake(MeshComp, HitPlayer, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
					HitPlayer->Attacked(AttackEnemy, MeshComp, HitResults[i].ImpactPoint, HitResults[i].ImpactNormal.Rotation());
				}
			}
		}		
	}
	
}

void UJAnimNotifyState_AttackValid::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (AttackInstigator == EAttackInstigator::EI_Player)
	{
		AttackPlayer = Cast<AJCharacter>(MeshComp->GetOwner());
		if (AttackPlayer)
		{
			Weapon = AttackPlayer->GetMesh();
			ActorsToIgnore = { MeshComp->GetOwner() };
		}
	}

	if (AttackInstigator == EAttackInstigator::EI_Enemy)
	{
		AttackEnemy = Cast<AJAICharacter>(MeshComp->GetOwner());
		if (AttackEnemy)
		{
			Weapon = AttackEnemy->GetMesh();
			ActorsToIgnore = { MeshComp->GetOwner() };
		}
	}
}

void UJAnimNotifyState_AttackValid::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitPlayers.Empty();
	HitEnemies.Empty();
}
