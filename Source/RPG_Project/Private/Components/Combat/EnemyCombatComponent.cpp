// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamplayTag.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappingActors.Contains(HitActor))
	{
		return;
	}
	OverlappingActors.AddUnique(HitActor);

	//处理玩家格挡
	//格挡是否有效
	bool bISValidBlock=false;

	//玩家是否格挡
	const bool bIsPlayerBlocking=false;
	//敌人攻击是否能够格挡
	const bool bIsMyAttackUnBlockable=false;

	//如果玩家进行格挡且敌人攻击能够格挡,检测格挡是否有效
	if (bIsPlayerBlocking&&!bIsMyAttackUnBlockable)
	{

	}
	//如果格挡有效，处理格挡
	if (bISValidBlock)
	{

	}

	else
	{
		FGameplayEventData Data;
		Data.Instigator=GetOwningPawn();
		Data.Target=HitActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), WarriorGameplayTags::Shared_Event_MeleeHit,Data);


	}


}

void UEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{

}
