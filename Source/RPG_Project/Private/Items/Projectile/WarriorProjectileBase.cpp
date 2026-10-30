// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Projectile/WarriorProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorGamplayTag.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AWarriorProjectileBase::AWarriorProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox=CreateDefaultSubobject<UBoxComponent>("ProjectileCollisionBox");
	SetRootComponent(ProjectileCollisionBox);

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this,&ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnProjectileBeginOverlap);


	ProjectileNiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>("ProjectileNiagaraComponent");
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	//UProjectileMovementComponent 是虚幻引擎中一个专门用于驱动子弹、投掷物、魔法弹等抛射物运动的组件
	ProjectileMovementComponent=CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed=700.f;
	ProjectileMovementComponent->MaxSpeed=900.f;
	//Velocity代表当前速度向量
	ProjectileMovementComponent->Velocity=FVector(1.f,0.f,0.f);
	//重力系数，这里为0代表抛射物不受重力影响
	ProjectileMovementComponent->ProjectileGravityScale=0.f;

	//Actor的一个属性,设置Actor在生成多少秒后自动销毁
	InitialLifeSpan=4.f;


}

void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if (ProjectileDamagePolicy==EProjectileDamagePolicy::OnBeginOverlap)
	{
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	}

}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);
	APawn* HitPawn=Cast<APawn>(OtherActor);

	if (!HitPawn||!UWarriorBlueprintFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
	{
		Destroy();
		return;
	}

	bool bISValidBlock=false;

	const bool bIsPlayerBlocking=UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(HitPawn,WarriorGameplayTags::Player_Status_Block);



	if (bIsPlayerBlocking)
	{
		bISValidBlock=UWarriorBlueprintFunctionLibrary::IsBlockValid(GetInstigator(),HitPawn);
	}
	FGameplayEventData EventData;
	EventData.Instigator=GetInstigator();
	EventData.Target=HitPawn;

	//如果格挡有效，处理格挡
	if (bISValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn, WarriorGameplayTags::Player_Event_SuccessfulBlock,EventData);

	}
	else
	{
		HandleApplyProjectileDamage(HitPawn,EventData);
	}


		Destroy();


}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverLapActors.Contains(OtherActor))
	{
		return;
	}
	OverLapActors.AddUnique(OtherActor);
	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		FGameplayEventData EventData;
		EventData.Instigator=GetInstigator();
		EventData.Target=HitPawn;

		if (UWarriorBlueprintFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn,EventData);
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad)
{
	check(ProjectileDamageEffectSpecHandle.IsValid());

	const bool bWasApplied=UWarriorBlueprintFunctionLibrary::ApplyGameplayEffectSpecHandleToTarget(GetInstigator(),InHitPawn,ProjectileDamageEffectSpecHandle);

	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InHitPawn,WarriorGameplayTags::Shared_Event_HitReact,InPayLoad);
	}
}





