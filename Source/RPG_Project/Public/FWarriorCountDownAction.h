// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"



//FLatentInfo（在C++中通常指 FLatentActionInfo）是虚幻引擎中用于处理异步潜伏操作（Latent Action） 的核心结构体。它就像一张"任务回执"，告诉引擎当异步操作（如延迟、加载）完成后，应该去找谁、执行哪个函数，以及从哪个点恢复执行。
//FLatentInfo的核心成员：CallbackTarget：指定了异步操作完成后，由哪个对象来执行回调函数，类型TObjectPtr<UObject>
//ExecutionFunction：回调的函数名，指定了异步操作完成后，要调用的具体函数名称，以字符串（FName）的形式存储，这个目标函数必须是 CallbackTarget 的成员函数，并且通常必须标记为 UFUNCTION
//Linkage 就是用来告诉引擎在操作完成后，应该从哪个特定的输出引脚继续执行后面的逻辑，在C++中手动创建 FLatentActionInfo 而用不到 Linkage，通常可以将其设为 0，但绝对不能设为 -1（-1 被保留为无效值），in32类型
class  FWarriorCountDownAction: public FPendingLatentAction
{
public:
	FWarriorCountDownAction(float InTotalCountDownTime,float InUpdateInterval,float& InOutRemainingTime,EWarriorCountDownActionOutput& InCountDownOutput,const FLatentActionInfo& LatentActionInfo)
	:bNeedToCancel(false)
	,TotalCountDownTime(InTotalCountDownTime)
	,UpdateInterval(InUpdateInterval)
	,OutRemainingTime(InOutRemainingTime)
	,CountDownOutput(InCountDownOutput)
	,ExecutionFunction(LatentActionInfo.ExecutionFunction)
	,OutputLink(LatentActionInfo.Linkage)
	,CallbackTarget(LatentActionInfo.CallbackTarget)
	,ElapsedInterval(0.f)
	,ElapsedTimeSinceStart(0.f)
	{}

	void CancelAction();

	virtual void UpdateOperation(FLatentResponse& Response) override;
private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	//UpdateInterval表示每次更新的时间间隔
	float UpdateInterval;
	float& OutRemainingTime;
	EWarriorCountDownActionOutput& CountDownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	//ElapsedInterval表示从上一次触发更新以来经过的时间
	float ElapsedInterval;
	//ElapsedTimeSinceStart表示从开始经过的总时间
	float ElapsedTimeSinceStart;
};
