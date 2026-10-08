// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WArriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void OnAbilityInputPressed(const FGameplayTag& InputTag);
	void OnAbilityInputReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta=(Applylevel="1"))
	void GrantHeroWeaponAbility(const TArray<FWarriorHeroAbilitySet>&InDefaultWeaponAbility,int32 ApplyLevel ,TArray<FGameplayAbilitySpecHandle>& OnGrantedAbilitySpecHandles);

	//UPARAM(ref)蓝图引用传递
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

};
