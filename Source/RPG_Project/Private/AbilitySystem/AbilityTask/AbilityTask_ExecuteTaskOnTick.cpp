// Fill out your copyright notice in the Description page of Project Settings.


//AbilityTask是GAS处理异步逻辑的核心机制,也可以用于处理Tick逻辑，GA没有内置的Tick逻辑，官方推荐的就是使用AbilityTask辅助
//自定义 AbilityTask 通常遵循“静态工厂 -> 激活 -> 委托广播 -> 结束”的模式
#include "AbilitySystem/AbilityTask/AbilityTask_ExecuteTaskOnTick.h"

UAbilityTask_ExecuteTaskOnTick::UAbilityTask_ExecuteTaskOnTick()
{
	bTickingTask=true;
}

UAbilityTask_ExecuteTaskOnTick* UAbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	//模板辅助函数，用于在 C++ 中创建自定义 AbilityTask 的实例。
	UAbilityTask_ExecuteTaskOnTick* Node=NewAbilityTask<UAbilityTask_ExecuteTaskOnTick>(OwningAbility);

	return Node;
}

void UAbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
//任务向技能蓝图（或C++代码）广播委托（Delegate）之前，确认该任务所属的 GameplayAbility 是否仍然有效（存活且处于激活状态）
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityTickTask.Broadcast(DeltaTime);
	}
	else
	{
		//任务介绍必须调用
		EndTask();
	}
}
