// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarriorCharacterAnimInstance.h"
#include "WarriorDebugHelper.h"
#include "KismetAnimationLibrary.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter=Cast<AWarriorBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}

}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter||!OwningMovementComponent)
	{
		return;
	}
	GroundSpeed=OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration=OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	//直接使用kismet函数库提供的函数进行计算，需要在build.cs中添加对应模块
	//CalculateDirection函数，接收Actor线速度与旋转，返回一个float类型的度数
	LocomotionDirection=UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(),OwningCharacter->GetActorRotation());


}
