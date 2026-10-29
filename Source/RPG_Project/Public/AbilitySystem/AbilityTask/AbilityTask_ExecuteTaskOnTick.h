// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ExecuteTaskOnTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTickDelegate,float,DeltaTime);

/**
 *
 */
UCLASS()
class RPG_PROJECT_API UAbilityTask_ExecuteTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_ExecuteTaskOnTick();
	//静态工厂函数
	//HidePin="OwningAbility"，作用是在蓝图节点中，将名为 OwningAbility 的参数引脚隐藏起来,常与下面的DefaultToSelf="OwningAbility联用
	//DefaultToSelf="OwningAbility，参数（OwningAbility）的默认值应该自动取节点所在的“自身”（Self）上下文
	//BlueprintInternalUseOnly核心作用是：将该 C++ 函数暴露给蓝图，但不会出现在蓝图节点的右键搜索菜单（Palette）中,常与上面的两个联用
	//BlueprintInternalUseOnly或BlueprintInternalUseOnly=“true”写法相同
	//这里的属性宏是AbilityTask静态工厂函数声明的标准做法
	UFUNCTION(BlueprintCallable,Category="Warrior|AbilityTasks",meta=(HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly))
	static UAbilityTask_ExecuteTaskOnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);

	//~ Begin UGameplayTask Interface
	virtual void TickTask(float DeltaTime) override;
	//~ End UGameplayTask Interface

	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskTickDelegate OnAbilityTickTask;
};
