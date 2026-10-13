// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/HeroCombatComponent.h"

#include "WarriorGamplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Items/Weapon/WarriorHeroWeapon.h"
#include "WarriorTypes/WArriorStructTypes.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return  Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquipWeapon() const
{
	return  Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquipWeaponDamageAtLevel(float InLevel) const
{
	//FScalableFloat::GetValueAtLevel 是 GAS 中用于根据等级从曲线表（Curve Table）中动态计算数值的核心函数
	return GetHeroCurrentEquipWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{

	//利用OverlappingActors防止一次攻击可能对同一个对象产生的多次begin重叠被重复计算
	if (OverlappingActors.Contains(HitActor))
	{
			return;
	}
	OverlappingActors.AddUnique(HitActor);

	//FGameplayEventData,存储事件信息，用于SendGameplayEventToActor的第三个参数
	FGameplayEventData Data;
	Data.Instigator=GetOwningPawn();
	Data.Target=HitActor;

	//GAS 中用于从外部向指定 Actor 发送事件以触发其技能或逻辑的核心函数
	//发送的对象ACtor必须存在ASC，若该 Actor 没有 ASC，事件将不会被发送。
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), WarriorGameplayTags::Shared_Event_MeleeHit,Data);


}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{

}

