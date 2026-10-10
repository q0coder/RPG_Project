// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/StratUpData/DataAsset_StartUpDataBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
                                                              int32 ApplyLevel)
{
	check(InASCToGive);
	GrantAbilities(ActivateGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;
			//通过GetDefaultObject函数获取对应类的CDO
			UGameplayEffect* EffectCDO=EffectClass->GetDefaultObject<UGameplayEffect>();

			//ApplyGameplayEffectToSelf需要接收一个CDO
			//ApplyGameplayEffectToSelf用于便捷的将GE应用到ASC上，
			//需要更为灵活完整的功能可以使用ApplyGameplayEffectSpecToSelf
			//InASCToGive->MakeEffectContext() 的作用是：为即将应用的 GameplayEffect 创建一个“上下文句柄”，用来记录“这个效果是谁发的、对谁发的、怎么命中的”等元数据信息。
			InASCToGive->ApplyGameplayEffectToSelf(EffectCDO,ApplyLevel,InASCToGive->MakeEffectContext());
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
	UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
	{

		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASCToGive->GiveAbility(AbilitySpec);

	}
}

