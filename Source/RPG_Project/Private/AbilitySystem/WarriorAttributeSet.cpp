// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorDebugHelper.h"

#include "GameplayEffectExtension.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorGamplayTag.h"

UWarriorAttributeSet::UWarriorAttributeSet()
{
	//用于测试的设置
	InitAttackPower(1.f);
	InitCurrentHealth(1.f);
	InitCurrentRage(1.f);
	InitDefencePower(1.f);
	InitMaxHealth(1.f);
	InitMaxRage(1.f);

}
//GameplayEffectExecutionCalculation一般不直接处理属性的修改
void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute==GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(),0.f,GetMaxHealth());



		SetCurrentHealth(NewCurrentHealth);
	}

	if (Data.EvaluatedData.Attribute==GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(),0.f,GetMaxRage());
		SetCurrentHealth(NewCurrentRage);
	}
	if (Data.EvaluatedData.Attribute==GetDamageTakenAttribute())
	{


		const float OldHealth = GetCurrentHealth();
		const float DamageDone=GetDamageTaken();


		const float NewCurrentHealth = FMath::Clamp(OldHealth-DamageDone,0.f,GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		if (NewCurrentHealth==0.f)
		{
			UWarriorBlueprintFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGameplayTags::Shared_Status_Death);
		}

	}



}
