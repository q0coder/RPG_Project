// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AbilityTask_WaitSpawnEnemies.h"

#include "NavigationSystem.h"
#include "WarriorDebugHelper.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
                                                                               FGameplayTag EventTag,
                                                                               TSoftClassPtr<AWarriorEnemyCharacter>
                                                                               SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                               const FVector& SpawnOrigin,
                                                                               float RandomSpawnRadius
)
{
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;

	return Node;
}

//AddUObject 是“给 C++ 代码自己用”的轻量路径，AddDynamic 是“暴露给蓝图”的路径。
//AddUObject只适用于非动态委托（纯 C++），不经过UE的反射系统，性能较高，一般用于GAS 内部 C++ 逻辑（如 AbilityTask 监听事件）
void UAbilityTask_WaitSpawnEnemies::Activate()
{
	Super::Activate();
	//作用是向ASC 的事件系统注册一个“当 CachedEventTag 事件发生时，调用 this->OnGameplayEventReceive”的监听
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(
		CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceive);
}

//重写虚函数时，如果函数是用于初始化的Super必须最先调用，函数用于销毁清理的，Super必须后调
void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	//结束时撤销这条监听
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(
		CachedEventTag);
	Delegate.Remove(DelegateHandle);
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceive(const FGameplayEventData* InPayLoad)
{
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		//调用异步加载，核心参数包括异步加载对象与加载完成后的回调函数
		//异步加载负责“把磁盘上的字节变成内存里可用的资源对象
		//回调函数负生成具体的Actor，异步加载负责将磁盘中的资源加载到内存中
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
		);
	}
	else
	{
		//ShouldBroadcastAbilityTaskDelegates 是 Ability Task 在向外广播任何委托之前必须调用的一个“守卫”函数。它的核心作用是确保委托被广播时，其所属的 Gameplay Ability 依然处于激活状态，从而避免向一个已经结束的能力传递过期的异步结果。
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	//Get函数的作用是获取UClass类型
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();

	//异步加载的回调函数中必须检测World是否为空，异步加载期间，如果发生切图或 World 被销毁，this 所属的 UWorld 可能已经 CleanupWorld
	UWorld* World = GetWorld();

	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		EndTask();
		return;
	}

	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;
	//FActorSpawnParameters 是 SpawnActor 函数的一个可选参数结构体，用于在生成 Actor 时进行精细化的控制。把它想象成生成 Actor 时的“设置面板”，如果不传入，引擎会使用一套默认的生成规则
	FActorSpawnParameters SpawnParameters;
	// 碰撞处理 (SpawnCollisionHandlingOverride)，这里是尝试寻找附近不碰撞的位置，但无论如何都会生成
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < CachedNumToSpawn; ++i)
	{
		FVector RandomLocation;
		//在指定原点的一定半径内，随机获取一个导航网格（NavMesh）上可“到达”的点。
		//核心参数包括生成的中心点，输出的生成点，生成的半径
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation,
		                                                        CachedRandomSpawnRadius);

		RandomLocation += FVector(0.f, 0.f, 150.f);

		FRotator SpawnRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().
		                                                 ToOrientationRotator();
		//SpawnActor的参数，生成的对象，生成的对象的位置与旋转，生成规则（可选）
		AWarriorEnemyCharacter* SpawnEnemy = World->SpawnActor<AWarriorEnemyCharacter>(
			LoadedClass, RandomLocation, SpawnRotation, SpawnParameters);

		if (SpawnEnemy)
		{
			SpawnedEnemies.Add(SpawnEnemy);
		}


	}
	if (ShouldBroadcastAbilityTaskDelegates())
	{

		if (!SpawnedEnemies.IsEmpty())
		{

			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(SpawnedEnemies);
		}
	}

	EndTask();
}
