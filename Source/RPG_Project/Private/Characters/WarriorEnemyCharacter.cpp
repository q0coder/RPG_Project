// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"

#include "WarriorBlueprintFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Components/WidgetComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Components/UI/EnemyUIComponent.h"
#include "DataAsset/StratUpData/DataAsset_StartUpDataBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include  "Engine/AssetManager.h"

AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	// AutoPossessAI是APawn 类的一个成员变量。它决定了当这个 Pawn 出现在游戏世界中时，是否自动生成并绑定一个 AI 控制器。
	//EAutoPossessAI是一个枚举，表示生成AI控制器的规则
	//存在四个枚举值，Disabled（禁用）：无论怎么出现，都不会自动生成 AI 控制器，需要我们手动生成AIController
	//PlacedInWorld（仅放置在场景中）：只有当你在编辑器里手动把这个角色拖拽到关卡场景中时，它才会自动生成 AI 控制器并控制自己。（这是 UE 的默认值）。
	//Spawned（仅动态生成）：只有当你在游戏运行时，通过代码（如 SpawnActor）动态生成这个角色时，它才会自动生成 AI 控制器。
	//PlacedInWorldOrSpawned（放置在场景中或动态生成）
	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;

	//让角色平滑地转向控制器（摄像机）期望的旋转方向与bOrientRotationToMovement互斥
	GetCharacterMovement()->bUseControllerDesiredRotation=false;
	//让角色的模型朝向自动跟随移动方向旋转
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.f,180.f,0.f);
	GetCharacterMovement()->MaxWalkSpeed=300.f;
	//UE为角色设定了默认的加速度与减速度为2048
	GetCharacterMovement()->BrakingDecelerationWalking=1000.f;

	EnemyCombatComponent=CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));
	EnemyUIComponent=CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));
	EnemyHealthWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandBoxCollision=CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBoxCollision"));
	LeftHandBoxCollision->SetupAttachment(GetMesh());
	LeftHandBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightHandBoxCollision=CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBoxCollision"));
	RightHandBoxCollision->SetupAttachment(GetMesh());
	RightHandBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnBodyCollisionBoxBeginOverlap);



}

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}

//PostEditChangeProperty函数的执行逻辑就是，我们将附加的骨骼名称设置为了EditDefaultsOnly，可以在编辑器类默认值的细节面板中进行修改
//EditDefaultsOnly函数在我们修改类默认值细节面板时就会调用，将碰撞体附加到对应的骨骼上，整个过程完成在编辑器中完成，在运行时本就不应该保留
#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//GET_MEMBER_NAME_CHECKED用于安全地获取某个类成员变量/函数的 FName，同时在编译期做类型检查。
	if (PropertyChangedEvent.GetMemberPropertyName()==GET_MEMBER_NAME_CHECKED(ThisClass,LeftHandBoxCollisionAttachBoneName))
	{
		//吸附到目标，不包含缩放
		LeftHandBoxCollision->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,LeftHandBoxCollisionAttachBoneName);

	}
	if (PropertyChangedEvent.GetMemberPropertyName()==GET_MEMBER_NAME_CHECKED(ThisClass,RightHandBoxCollisionAttachBoneName))
	{
		RightHandBoxCollision->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,RightHandBoxCollisionAttachBoneName);

	}
}
#endif

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;

}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UWarriorWidgetBase* HealthWidget=Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreateWidget(this);
	}

}

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		if (UWarriorBlueprintFunctionLibrary::IsTargetPawnHostile(this,HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}



void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull()) return;
	//通过 UAssetManager 获取FStreamableManager，调用异步加载
	//两个核心参数，一个路径容器，一个回调函数（在加载完成的下一帧调用）
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		//用于把一个 Lambda 表达式包装成 FStreamableDelegate，然后传给 RequestAsyncLoad 作为加载完成后的回调
		//FStreamableDelegate无参，无返回值，Lambda也必须相同

		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadData=CharacterStartUpData.Get())
				{
					LoadData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
				}
			}
		)
	);

}
