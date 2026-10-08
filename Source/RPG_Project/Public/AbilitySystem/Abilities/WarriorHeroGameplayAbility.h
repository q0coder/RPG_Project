// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"

class UHeroCombatComponent;
class AWarriorHeroController;
class AWarriorHeroCharacter;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
public:
	//从GameAbility的ActorInfo中获取HeroCharacter玩家角色
	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo() ;

	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	AWarriorHeroController* GetHeroControllerFromActorInfo() ;

	UFUNCTION(BlueprintPure, Category="WarriorAbility")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo() ;

private:
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;

};
