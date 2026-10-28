// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamplayTag.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/BoxComponent.h"

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
	const bool bIsPlayerBlocking=UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(HitActor,WarriorGameplayTags::Player_Status_Block);
	//敌人攻击是否能够格挡
	const bool bIsMyAttackUnBlockable=UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(HitActor,WarriorGameplayTags::Enemy_Status_UnBlockable);

	//如果玩家进行格挡且敌人攻击能够格挡,检测格挡是否有效
	if (bIsPlayerBlocking&&!bIsMyAttackUnBlockable)
	{
		bISValidBlock=UWarriorBlueprintFunctionLibrary::IsBlockValid(GetOwningPawn(),HitActor);
	}
	FGameplayEventData EventData;
	EventData.Instigator=GetOwningPawn();
	EventData.Target=HitActor;

	//如果格挡有效，处理格挡
	if (bISValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, WarriorGameplayTags::Player_Event_SuccessfulBlock,EventData);

	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), WarriorGameplayTags::Shared_Event_MeleeHit,EventData);
	}


}

void UEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{

}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	Super::ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	AWarriorEnemyCharacter* OwningEnemyCharacter=Cast<AWarriorEnemyCharacter>(GetOwner());

	check(OwningEnemyCharacter);

	UBoxComponent* LeftHandBoxCollision=OwningEnemyCharacter->GetLeftHandBoxCollisionComponent();
	UBoxComponent* RightHandBoxCollision=OwningEnemyCharacter->GetRightHandBoxCollisionComponent();

	check(RightHandBoxCollision&&LeftHandBoxCollision);

	switch (ToggleDamageType)
	{

	case EToggleDamageType::LeftHand:
		LeftHandBoxCollision->SetCollisionEnabled(bShouldEnable?ECollisionEnabled::QueryOnly:ECollisionEnabled::NoCollision);
		break;
	case EToggleDamageType::RightHand:
		RightHandBoxCollision->SetCollisionEnabled(bShouldEnable?ECollisionEnabled::QueryOnly:ECollisionEnabled::NoCollision);
		break;
	default:
		break;
	}

	if (!bShouldEnable)
	{
		OverlappingActors.Empty();
	}

}
