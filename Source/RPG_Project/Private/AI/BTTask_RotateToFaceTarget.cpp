// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RotateToFaceTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName=TEXT("Native Rotate To Face Target Actor");
	AnglePrecision=10.f;
	RotationInterpSpeed=5.f;

	//bNotifyTick不设置它为 true，即使 ExecuteTask 返回了 InProgress，引擎也不会调用TickTask 函数。
	bNotifyTick=true;
	//激活 OnTaskFinished 回调函数
	bNotifyTaskFinished=true;
	//不要为每个 AI 单独实例化这个节点的类对象，请使用共享模板 + 内存块的方式,这也是默认的方式，默认就是false
	bCreateNodeInstance=false;

	//宏，作用就是设置上面的三个标志位，有了这个宏上面的三个可直接省略
	INIT_TASK_NODE_NOTIFY_FLAGS();

	////AddObjectFilter用于约束黑板键可接受对象类型
	InTargetToFaceKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(ThisClass,InTargetToFaceKey),AActor::StaticClass());
}

//获取黑板键的固定操作
void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset=GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(UBTTask_RotateToFaceTarget);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription=InTargetToFaceKey.SelectedKeyName.ToString();
	//SanitizeFloat用于将浮点型转化为FString
	return FString::Printf(TEXT("Smoothly rotate to face %s key until the angle precision: %s is reached"),*KeyDescription,*FString::SanitizeFloat(AnglePrecision));

}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject=OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor=Cast<AActor>(ActorObject);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	FRotateToFaceTargetTaskMemory* Memory=CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	Memory->OwningPawn=OwningPawn;
	Memory->TargetActor=TargetActor;

	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	//判断旋转的方位是否已经达标
	if (HasReachedAnglePrecision(OwningPawn,TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	FRotateToFaceTargetTaskMemory* Memory=CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	if (!Memory->IsValid())
	{
		//FinishLatentTask结束一个“延迟任务” ,任务完成之后必须Finish才能继续执行，否则卡住
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
	}

	//Get可以用于从智能指针中获取原始指针，这里的OwningPawn与TargetActor前面定义的是弱引用指针
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(),Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(),Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot= FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(),LookAtRot,DeltaSeconds,RotationInterpSpeed);

		Memory->OwningPawn->SetActorRotation(TargetRot);
	}

}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerForward=QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized=(TargetActor->GetActorLocation()-QueryPawn->GetActorLocation()).GetSafeNormal();

	//DotProduct进行点乘，DegAcos将点乘的结果换算成角度度数
	const float DotResult=FVector::DotProduct(OwnerForward,OwnerToTargetNormalized);
	const float AngleDiff=UKismetMathLibrary::DegAcos(DotResult);

	return AngleDiff<AnglePrecision;
}
