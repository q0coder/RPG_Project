// Fill out your copyright notice in the Description page of Project Settings.


#include "FWarriorCountDownAction.h"

#include "WarriorDebugHelper.h"

void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel=true;
}

void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{

	if (bNeedToCancel)
	{
		CountDownOutput=EWarriorCountDownActionOutput::Canceled;

		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}

	if (ElapsedTimeSinceStart>=TotalCountDownTime)
	{

		CountDownOutput=EWarriorCountDownActionOutput::Completed;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}

	if (ElapsedInterval<UpdateInterval)
	{

		//Response.ElapsedTime()返回的是从上一次更新到本次更新之间的时间增量
		ElapsedInterval+=Response.ElapsedTime();
	}

	else
	{

		ElapsedTimeSinceStart+=UpdateInterval>0.f?UpdateInterval:Response.ElapsedTime();

		OutRemainingTime=TotalCountDownTime-ElapsedTimeSinceStart;

		CountDownOutput=EWarriorCountDownActionOutput::Update;

		Response.TriggerLink(ExecutionFunction,OutputLink,CallbackTarget);

		ElapsedInterval=0.f;
	}

}
