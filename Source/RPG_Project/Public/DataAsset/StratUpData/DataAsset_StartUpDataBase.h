// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UGameplayEffect;
class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1) ;

protected:
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateGivenAbilities;

	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	//TSubclassOf就是UClass*的一个封装，它只存储引用，不存储具体的实例（CDO）
	//所有非纯C++类在引擎运行时都会产生对应的实例，称为CDO（Class Default Object），
	//二裸指针可以存储对象实例，当使用裸指针，在编辑器中配置时我们就需要配置CDO
	//使用TSubclassOf，我们在编辑器中配置时就可以配置类，而非CDO
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;

	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1);
};
