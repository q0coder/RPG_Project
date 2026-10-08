// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//这里GameplayTag是作为值传递的，不能使用前项声明
#include "GameplayTagContainer.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"


class AWarriorWeaponBase;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
public :
	//注册生成的武器到一个TMap，参数1武器的Tag，参数2武器对象，参数3是否注册为当前装备的武器
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister,AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquippedWeapon=false);

	//通过Tag获取角色携带的武器
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	//当前装备的武器的Tag
	UPROPERTY(BluePrintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	//获取角色当前装备的武器
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;
private:
	//GamplayTag与武器类的映射（包括玩家与敌人的武器）
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
};
