// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorBlueprintFunctionLibrary.generated.h"

struct FScalableFloat;
class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;

/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//代码本地函数
	static UWarriorAbilitySystemComponent* NativeGetWarriorAbilitySystemComponentFromActor(AActor* InActor);
	UFUNCTION(BlueprintCallable,Category="Warrior|FunctionLibrary")
	//给角色的ASC添加Tag
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	//移除角色ASC的Tag
	UFUNCTION(BlueprintCallable,Category="Warrior|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	//判断ASC是否存在Tag
	static  bool NativeDoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck);

	//ExpandEnumAsExecs元数据说明符的作用是在编译时根据枚举的类型生成多个执行引脚
	UFUNCTION(BlueprintCallable,Category="Warrior|FunctionLibrary",meta=(DisplayName="Does Actor Have Tag",ExpandEnumAsExecs="OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,EWarriorConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponent(AActor* InActor);

	UFUNCTION(BlueprintCallable,Category="Warrior|FunctionLibrary",meta=(DisplayName="Get Pawn Combat Component From Actor",ExpandEnumAsExecs="OutValidType"))
	static  UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType);

	UFUNCTION(BlueprintPure,Category="Warrior|FunctionLibrary")
	static  bool IsTargetPawnHostile(APawn* QueryPawn,APawn* TargetPawn) ;

	//meta=(CompactNodeTitle，元数据说明符常与BlueprintPure或BlueprintCallable 联用，用于将蓝图中的函数节点简化
	UFUNCTION(BlueprintPure,Category="Warrior|FunctionLibrary",meta=(CompactNodeTitle="Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat,float InLevel=1.f);


};
