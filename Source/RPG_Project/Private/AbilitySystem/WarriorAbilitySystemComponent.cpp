// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include  "WarriorDebugHelper.h"
void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()){return;}

	for (const FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

		TryActivateAbility(AbilitySpec.Handle);

	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{

}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbility(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbility,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OnGrantedAbilitySpecHandles )
{
	if (InDefaultWeaponAbility.IsEmpty()) {return;}

	for (const FWarriorHeroAbilitySet& AbilitySet:InDefaultWeaponAbility)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		GiveAbility(AbilitySpec);

		OnGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));

	}


}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()){return;}

	for (const FGameplayAbilitySpecHandle& SpecHandle:InSpecHandlesToRemove)
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
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{

		const int32 RandomAbilityIndex=FMath::RandRange(0,FoundAbilitySpecs.Num()-1);
		FGameplayAbilitySpec* SpecToActivate=FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{

			//TryActivateAbility通过技能句柄激活技能
			return  TryActivateAbility(SpecToActivate->Handle);
		}

	}

	return false;
}
