// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"

#include "Components/Combat/EnemyCombatComponent.h"
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

	CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");

}

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
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
