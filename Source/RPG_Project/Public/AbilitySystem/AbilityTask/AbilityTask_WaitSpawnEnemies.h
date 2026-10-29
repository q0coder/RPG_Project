// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

//：UHT（Unreal Header Tool）在解析反射代码时，不允许 TArray 直接包裹一个裸的 UObject 类名，它要求必须是 TArray<UObject*> 或 TArray<UObject类名*>。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<AWarriorEnemyCharacter*>&,
                                            SpawnEnemies);

class AWarriorEnemyCharacter;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()

public:
	//RandomSpawnRadius为随机生成半径
	//UFUNCTION的元说明符中可以直接对函数的参数进行默认的赋值,其与函数参数默认值的C++语法的区别在于一个作用于编译器，一个作用于蓝图，且元说明符不受默认参数必须从参数列表的最右侧开始的C++语法限制
	UFUNCTION(BlueprintCallable, Category="Warrior|AbilityTask",
		meta=(DisplayName="Wait Gameplay Event And Spawn Enemies", HidePin="OwningAbility", DefaultToSelf=
			"OwningAbility", BlueprintInternalUseOnly, NumToSpawn="1", RandomSpawnRadius="200"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius,
		const FRotator& SpawnRoration
	);

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;

	FVector CachedSpawnOrigin;

	float CachedRandomSpawnRadius;

	FRotator CachedSpawnRoration;
};
