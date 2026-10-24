// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "EnhancedInputSubsystems.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamplayTag.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/SizeBox.h"

#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Controler/WarriorHeroControler.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUP();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	CurrentLockedActor = GetNearestTargetFormAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		DrawTargetLockedWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorToLock()
{

	AvailableActorsToLock.Empty();

	//ForObject的碰撞检测，检测场景中一定范围的敌人（锁定目标）
	TArray<FHitResult> BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() *
		BoxTraceDistance,
		TraceBoxSize / 2.f,
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistantDebugType ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	//取消能力
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed=GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed=TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(TargetLockMappingContext,3);

}

void UHeroGameplayAbility_TargetLock::CleanUP()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;

	if (DrawTargetLockWidget)
	{
		//RemoveFromViewPort已经废弃，使用RemoveFromParent代替
		DrawTargetLockWidget->RemoveFromParent();
	}
	DrawTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed=0.f;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed>0.f)
	{
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed=CachedDefaultMaxWalkSpeed;

	}

}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	//如果不进行该判断，在游戏开始时调用Init，之后直接结束游戏播放，可能会导致无法获取到Controller
	//原因在于，游戏结束时，会按顺序销毁场景中的对象，LocalPlayer 和 PlayerController 是首先被清理的对象之一
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->RemoveMappingContext(TargetLockMappingContext);

}

void UHeroGameplayAbility_TargetLock::DrawTargetLockedWidget()
{
	if (!DrawTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forget to assign a valid widget in Blueprint"));

		//创建Widget的工厂函数，只负责创建不负责显示
		//参数1，OwningObject必须是UWidget 或其派生类（用于嵌套控件），UWidgetTree，APlayerController 或其派生类，APlayerController 或其派生类， UGameInstance 或其派生类UWorld 或其派生类
		//不能直接在AActor及其衍生类中直接使用This作为第一个参数调用，会报错
		DrawTargetLockWidget = CreateWidget<
			UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawTargetLockWidget);

		DrawTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	//MG 系统中用于将 3D 世界坐标转换为 2D 屏幕 UI 坐标的核心函数。它常见于血条、标记、伤害数字等需要让 UI 元素跟随 3D 物体的场景。
	//UGameplayStatics::ProjectWorldToScreen 也能将世界坐标转换为屏幕坐标
	//普通的 ProjectWorldToScreen 返回的是原始的屏幕像素坐标。而 UMG 系统拥有自己独立的坐标系，会受到 DPI 缩放（DPI Scaling） 和视口缩放（Viewport Scaling） 的影响
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true
	);

	//UWidgetTree 本身并不是一个可见的控件（它继承自 UObject），而是一个容器
	//在 C++ 中需要获取蓝图中创建的控件，但没有使用 BindWidget（即没有把它声明为 C++ 类的 UPROPERTY 成员）时，可以通过 WidgetTree 按名字查找
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		//用于高效地遍历一个 UserWidget 内部所有的子控件。接收一个 Lambda 表达式,会遍历该WidgetTree的所有子控件，对每一个子控件执行该Lambda表达式
		DrawTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					//GetWidthOverride与GetHeightOverride，SizeBox的函数，用于获取设置的重载宽度与高度
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}
	//ProjectWorldLocationToWidgetPosition函数默认返回的位置是给锚点（Anchor）在左上角（Top-Left）的控件使用的
	//锚点在中心（或其他位置）：你需要手动进行补偿计算。
	//如果希望让控件的中心对准返回的坐标点，必须用 返回值 - 控件尺寸 / 2 来调整最终位置
	ScreenPosition -= (TargetLockWidgetSize / 2.f);


	//用于在运行时将 UI 控件精确地放置到屏幕的指定 2D 坐标位置，通常就与ProjectWorldLocationToWidgetPosition一起使用
	//与ProjectWorldLocationToWidgetPosition使用，参数2必须设置为false
	DrawTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

//判断对于当前锁定的对象，谁在其左边，谁在右边
void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor||AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	const FVector PlayerLocation=GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized=(CurrentLockedActor->GetActorLocation()-PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor:AvailableActorsToLock)
	{
		if (!AvailableActor||AvailableActor==CurrentLockedActor) continue;;

		const FVector PlayerToAvailableNormalized=(AvailableActor->GetActorLocation()-PlayerLocation).GetSafeNormal();
		const FVector CrossResult=FVector::CrossProduct(PlayerToCurrentNormalized,PlayerToAvailableNormalized);

		if (CrossResult.Z>0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}

		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFormAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors,
	                                          ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor,
		                                                         WarriorGameplayTags::Shared_Status_Death) ||
		UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),
		                                                         WarriorGameplayTags::Shared_Status_Death))
	{
		CancelTargetLockAbility();
		return;
	}
	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation = !UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(
		GetHeroCharacterFromActorInfo(),
		WarriorGameplayTags::Player_Status_Rolling) && !UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(
		GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Block);

	if (bShouldOverrideRotation)
	{
		 FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(), CurrentLockedActor->GetActorLocation());

		LookAtRot-=FRotator(TargetLockCameraOffestDistance,0.f,0.f);

		const FRotator CurrentControlRot = GetHeroControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime,
		                                            TargetLockRotateInterpSpeed);

		//修改玩家角色的旋转与摄像机的旋转
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(FRotator(0.f, TargetRot.Yaw, 0)));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorToLock();
	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock=nullptr;

	GetAvailableActorsAroundTarget(ActorsOnLeft,ActorsOnRight);

	if (InSwitchDirectionTag==WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock=GetNearestTargetFormAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock=GetNearestTargetFormAvailableActors(ActorsOnRight);
	}
	if (NewTargetToLock)
	{
		CurrentLockedActor=NewTargetToLock;
	}
}
