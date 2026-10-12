// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "WarriorGamplayTag.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Controler/WarriorHeroControler.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		CachedWarriorHeroCharacter=Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarriorHeroCharacter.IsValid()? CachedWarriorHeroCharacter.Get():nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController=Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	return CachedWarriorHeroController.IsValid()? CachedWarriorHeroController.Get():nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);

	//FGameplayEffectContextHandle类型变量，用于记录相关信息，MakeOutgoingSpecs的参数
	FGameplayEffectContextHandle ContextHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	//AddInstigator的两个参数，分别添加发起者与来源者
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());

	//从底层看UGameplayEffect（GE 资产/类）不能直接生效，真正被应用到目标身上的是 FGameplayEffectSpec。
	//我们前面使用的ApplyGameplayEffectToSelf实际上是一种便捷的函数，其底层实际上也是创建Spec，再应用到目标上的
	//手动构造FGameplayEffectSpec提供了更为灵活的应用方式，一般只有静态的初始数据会直接使用ApplyGameplayEffectToSelf（如我们前面设置的最大血量等）
	//MakeOutgoingSpecs的作用就是从一个GameplayEffect类中创建一个FGameplayEffectSpec
	FGameplayEffectSpecHandle EffectSpecHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
	EffectClass,
	GetAbilityLevel(),
	ContextHandle
	);

	//SetSetByCallerMagnitude可用于在应用效果前，将外部数据注入到效果规格（Spec）中，
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage,InWeaponBaseDamage);
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag,InUsedComboCount);
	}

	return EffectSpecHandle;


}
