// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "WarriorBaseAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	//对于BlueprintPure这里蓝图纯函数，一般都需要与const联用，表示不修改自身数据”的纯读取节点，没有执行引脚
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;

};
