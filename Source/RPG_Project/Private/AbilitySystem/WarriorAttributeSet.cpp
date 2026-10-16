// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorDebugHelper.h"

#include "GameplayEffectExtension.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorGamplayTag.h"
#include "Components/UI/HeroUIComponent.h"
#include "Components/UI/PawnUIComponent.h"
#include "Interface/PawnUIInterface.h"

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
	//GetAvatarActor返回的是基类类型AActor，是一个编译期间的静态类型
	//Cast运行期查询，它能够获取运行时真实的类型，所以能够将AActor转化为IPawnInterface
	//IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(Data.Target.GetAvatarActor());


	//为了防止每次调用都定义一个PawnUIInterface，可以将其缓存起来
	//这里缓存了接口指针，实际上UPawnUIComponent同样可以缓存，但常见的逻辑是只缓存接口
	//缓存必须使用弱引用指针，否则在对象销毁后可能会导致野指针
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface=TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	//GetActorNameOrLabel用于获取一个适合在调试时展示的、人类可读的 Actor 名称
	checkf(CachedPawnUIInterface.IsValid(),TEXT("%s didn't implement IPawnUIInterface"),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	UPawnUIComponent* PawnUIComponent=CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent,TEXT("Couldn't extract a PawnUIComponent From %s"),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute==GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(),0.f,GetMaxHealth());



		SetCurrentHealth(NewCurrentHealth);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());

	}

	if (Data.EvaluatedData.Attribute==GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(),0.f,GetMaxRage());
		SetCurrentHealth(NewCurrentRage);
		if (UHeroUIComponent* HeroUIComponent=CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage()/GetMaxRage());
		}

	}
	if (Data.EvaluatedData.Attribute==GetDamageTakenAttribute())
	{


		const float OldHealth = GetCurrentHealth();
		const float DamageDone=GetDamageTaken();


		const float NewCurrentHealth = FMath::Clamp(OldHealth-DamageDone,0.f,GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());

		if (GetCurrentHealth()==0.f)
		{
			UWarriorBlueprintFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGameplayTags::Shared_Status_Death);
		}

	}



}
