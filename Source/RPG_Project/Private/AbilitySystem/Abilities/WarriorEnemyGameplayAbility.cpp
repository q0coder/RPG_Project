// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

#include "WarriorGamplayTag.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorEnemyCharacter.h"

AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
		if (!CachedWarriorEnemyCharacter.IsValid())
		{
			CachedWarriorEnemyCharacter=Cast<AWarriorEnemyCharacter>(CurrentActorInfo->AvatarActor);
		}
		return CachedWarriorEnemyCharacter.IsValid()? CachedWarriorEnemyCharacter.Get():nullptr;

}

UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UWarriorEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageFScalableFloat)
{
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());


	FGameplayEffectSpecHandle EffectSpecHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
	EffectClass,
	GetAbilityLevel(),
	ContextHandle
	);


	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage,InDamageFScalableFloat.GetValueAtLevel(GetAbilityLevel()));



	return EffectSpecHandle;

}
