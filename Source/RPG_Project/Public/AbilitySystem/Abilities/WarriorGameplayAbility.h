// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorGameplayAbility.generated.h"

class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;

UENUM(BlueprintType	)
enum class EWarriorAbilityActivationPolicy:uint8
{
	OnTrigger,
	OnGiven
};

UCLASS()
class RPG_PROJECT_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	//~ Begin UGameplayAbility interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override ;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	//~ End UGameplayAbility interface

	UPROPERTY(EditDefaultsOnly,Category="WarriorAbility")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy=EWarriorAbilityActivationPolicy::OnTrigger;

	//从GameAbility的ActorInfo中获取PawnCombatComponent组件。
	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	//从GameAbility的ActorInfo中获取ASC。
	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;

};
