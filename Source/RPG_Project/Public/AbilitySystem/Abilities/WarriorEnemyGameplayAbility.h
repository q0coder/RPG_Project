// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"

class UEnemyCombatComponent;
class AWarriorEnemyCharacter;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
public:
	//从GameAbility的ActorInfo中获取HeroCharacter玩家角色
	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo() ;

	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo() ;

private:
	//CachedWarriorEnemyCharacter弱引用不应该影响AWarriorEnemyCharacter的销毁
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedWarriorEnemyCharacter;

};
