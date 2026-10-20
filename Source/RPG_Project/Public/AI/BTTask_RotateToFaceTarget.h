// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToFaceTarget.generated.h"

struct  FRotateToFaceTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	bool IsValid() const
	{
		return OwningPawn.IsValid()&&TargetActor.IsValid();
	}

	//节点内存的重置，写自定义 Task 时必须手动处理的重置
	//原因在于这里引擎采用内存池的设计来分配内存，Reset的作用是将数据清除，而非回收内存
	//如果不进行数据的清除，那么可能会形成脏数据，导致后面使用时使用到原先为清除的数据
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};


/**
 *
 */
UCLASS()
class RPG_PROJECT_API UBTTask_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_RotateToFaceTarget();

protected:
	//~ Begin UBTTaskNode Interface
	//InitializeFromAsset初始化任何与资产相关的数据,每次行为树实例化时都会被执行,调用发生在节点复制与内存分配之后
	virtual  void InitializeFromAsset(UBehaviorTree& Asset) override;
	//GetInstanceMemorySize内存分配，紧接着节点复制调用，为每一个AI实例分配内存用于存储运行时状态
	virtual  uint16 GetInstanceMemorySize() const override;

	virtual  FString GetStaticDescription() const override;
	//~ End UBTTaskNode Interface

	//执行任务时调用该函数，存在四种枚举返回值（Succeeded，Failed，InProgress（运行中），Aborted（已终止）），决定行为树接下来的执行流程
	//两个参数，1表示整个行为树组件，通过它可以获取AIController、Pawn 以及黑板组件，参数2指向GetInstanceMemorySize分配的专属内存块，可以访问其
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//只有当 ExecuteTask 返回了 EBTNodeResult::InProgress 时，引擎才会在后续的每一帧调用该节点的 TickTask
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool HasReachedAnglePrecision(APawn* QueryPawn,AActor* TargetActor) const;


private:
	//感知角度，其实就是旋转的误差角度
	UPROPERTY(EditAnywhere,Category="Face Target")
	float AnglePrecision;

	UPROPERTY(EditAnywhere,Category="Face Target")
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere,Category="Face Target")
	FBlackboardKeySelector InTargetToFaceKey;


};
