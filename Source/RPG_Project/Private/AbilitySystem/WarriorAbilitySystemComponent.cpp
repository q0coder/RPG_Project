// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include  "WarriorDebugHelper.h"
#include "WarriorGamplayTag.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) { return; }

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

		if (InputTag.MatchesTag(WarriorGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{
	//MatchesTag与HasTag的区别，MatchesTag会进行层级匹配，针对单个标签,只有是子标签就会返回true
	//HasTag针对标签容器，查找容器中是否存在对应标签或者其子标签，为层级匹配
	//HasTagExact才是精确匹配
	//这里的判断作用是将非长按的输入返回，防止非长按的输入进入下面后产生按下就取消的情况
	if (!InputTag.IsValid() || !InputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld))
	{
		return;
	}

	//DynamicAbilityTags（动态）：定义在 AbilitySpec 上，允许你为每个技能实例附加额外的标签。
	//例如，同一个“攻击”技能，实例A可以是 Ability.Attack.Fire，实例B可以是 Ability.Attack.Ice


	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			//CancelAbilityHandle作用是取消一个技能，属于ASC，可以在GA的外部取消技能，玩家受击了需要打断当前施法（被动、强制中断）
			//End Ability也能取消技能，属于GA，一般用于技能成功命中敌人（主动、正常下班）
			//EndAbility 是技能对自己说“我干完了”，而 CancelAbility 是系统对技能说“你别干了”。
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbility(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbility, int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OnGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbility.IsEmpty()) { return; }

	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbility)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);


		OnGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()) { return; }

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
		InSpecHandlesToRemove.Empty();
	}
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;


	//作用是找出包含此容器中所有标签的Ability Spec，参数1为标签容器，参数2找到的可激活技能的Spec
	//GetSingleTagContainer获取的是传入标签的所有子标签的容器
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),
	                                                    FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			//TryActivateAbility通过技能句柄激活技能
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}
