// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorGameInstance.h"

#include "MoviePlayer.h"

#include "WarriorDebugHelper.h"

void UWarriorGameInstance::Init()
{
	//CoreUObjectDelegates 是一个全局的静态结构体，它汇集了 CoreUObject 模块中一系列低层级的全局多播委托（Multicast Delegates）
	Super::Init();
	//PreLoadMap提供了一个在加载地图的最开始广播的委托，在每次进行地图加载的最开始该委托都会对其所有绑定的回调函数进行广播
	//PostLoadMapWithWorld在地图的加载流程结束之后进行广播，可能早于BeginPaly执行
	//这两个委托一般一起使用是处理地图加载的标准方式
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ThisClass::OnDestinationWorldLoaded);
}

void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{

	//LoadingScreenAttributes 是 MoviePlayer 模块的核心配置结构体，它本身不执行任何加载逻辑，定义了在阻塞式地图加载期间，你要向玩家展示什么内容
	FLoadingScreenAttributes LoadingScreenAttributes ;
	//bAutoCompleteWhenLoadingCompletes 地图加载完成之后是否立即关闭加载界面，false的话需要StopMovie()手动取消
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes=true;
	//MinimumLoadingScreenDisplayTime，最低展示时间
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime=2.f;
	//Slate 是 UE5 底层的、完全由 C++ 实现的 UI 框架,UMG实际上是它的一层封装，WidgetLoadingScreen是指定要展示的界面的核心
	//WidgetLoadingScreen 要求的就是一个纯粹的 Slate 控件（SWidget），而不是一个 UMG 的 UUserWidget，可以通过TakeWidget()将UMG转化为Slate
	//FLoadingScreenAttributes::NewTestLoadingScreenWidget()的作用是创建一个引擎提供的简单加载界面，一个可用于简单的加载界面与调试
	LoadingScreenAttributes.WidgetLoadingScreen=FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	//SetupLoadingScreen作用的展示加载界面
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UWarriorGameInstance::OnDestinationWorldLoaded( UWorld* LoadedWorld)
{
	//停止加载界面
	GetMoviePlayer()->StopMovie();

}

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetGameLevelByTag(FGameplayTag GameplayTag) const
{
	for (const FWarriorLevelSet& GameLevelSet:GameLevelSets)
	{
		if (!GameLevelSet.IsValid()) continue;

		if (GameLevelSet.LevelTag==GameplayTag)
		{

			return GameLevelSet.Level;
		}
	}
	return TSoftObjectPtr<UWorld>();
}
