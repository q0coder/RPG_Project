// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_OrientToTargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName=TEXT("Native Orient Rotation To Target Actor");

	//核心作用是根据你在C++类中实现的具体函数，自动开启对应的底层通知（Notify）标志位，从而确保你的服务逻辑能被行为树正确调用
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterpSpeed=5.f;
	Interval=0.f;
	RandomDeviation=0.f;

	//AddObjectFilter用于约束黑板键可接受对象类型，三个参数，1一般是this，2配合下面的宏，需要约束的成员变量的FName名称，3约束的能够选择的类型
	//GET_MEMBER_NAME_CHECKED宏的作用是安全获取一个类成员变量（或函数）的名字，并将其转换为 FName 类型，在编译期提供类型安全检查和重构支持
	//GET_MEMBER_NAME_CHECKED参数1获取的成员所在的类，2获取的成员的变量名称
	InTargetActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(ThisClass,InTargetActorKey),AActor::StaticClass());
}

//这里的Super必须调用，核心作用是初始化与行为树资产（Asset）相关的数据，最常见、最关键的用途就是处理黑板键（Blackboard Key）的绑定。
void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset=GetBlackboardAsset())
	{
		//根本任务是将你在编辑器中选择的黑板键“名称”，解析为运行时真正使用的“ID”和“类型”,必须在InitializeFromAsset中调用ResolveSelectedKey
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

//编辑行为树编辑器中显示该节点的描述信息
FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription=InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient rotation to %s key %s"),*KeyDescription,*GetStaticServiceDescription());
}

void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UObject* ActorObject=OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor=Cast<AActor>(ActorObject);

	APawn* OwningPawn=OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn&&TargetActor)
	{
		const FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(),TargetActor->GetActorLocation());
		//RInterpTo专用于旋转插值，不过实际上这里并不推荐使用RInterpTo进行插值，虽然这里可以通过将服务的调用频率设置为0来实现每帧调用
		//更好的做法是获取AIController，使用SetFocus
		const FRotator TargetRot=FMath::RInterpTo(OwningPawn->GetActorRotation(),LookAtRot,DeltaSeconds,RotationInterpSpeed);

		OwningPawn->SetActorRotation(TargetRot);
	}
}

