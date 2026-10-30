// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"



//AbilityTask 的动态多播委托是“任务对外公布的生命周期信号接口”：它只定义签名、只负责在正确时机把结果广播出去，不承载任何具体逻辑。
///AbilityTask 的动态多播委托在蓝图中表现为执行引脚，参数表现为输出参数，代码在调用BoardCast广播动态多播委托后，执行引脚就会执行，执行引脚后面连接的函数会隐式的绑定委托
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
		float RandomSpawnRadius

	);

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface


private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;

	FVector CachedSpawnOrigin;

	float CachedRandomSpawnRadius;


	FDelegateHandle DelegateHandle;

	void OnGameplayEventReceive(const FGameplayEventData* InPayLoad);

	void OnEnemyClassLoaded();
};
