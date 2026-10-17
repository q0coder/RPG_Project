// Fill out your copyright notice in the Description page of Project Settings.


#include "Controler/WarriorAIController.h"
#include "WarriorDebugHelper.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


//C++ 中启用 Detour Crowd 群体避障（Detour Crowd Avoidance）的底层核心实现方式。
//:Super是C++初始化列表，Super作用是调用父类AIController的构造
//类 AAIController 默认会创建一个名为 "PathFollowingComponent"（寻路跟随组件）的子对象，其默认类型是 UPathFollowingComponent。
//这里Super传参的作用是创建PathFollowingComponent组件时，使用引擎的UCrowdFollowingComponent代替引擎提供的UPathFollowingComponent
//UPathFollowingComponent是UCrowdFollowingComponent的基类，只能解决简单的纯寻路AI逻辑
AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	//UAISenseConfig_Sight 本身不执行检测，而是一个配置容器，用来告诉 UAIPerceptionComponent（AI的感知组件）应该如何“看”世界。
	//DetectionByAffiliation这个属性是一个结构体，决定了AI的视觉会“看见”并处理哪些阵营的目标。
	//bDetectEnemies侦测敌人，bDetectFriendlies侦测友军，bDetectNeutrals侦测中立
	AiSenseConfig_Sight=CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AiSenseConfig_Sight->DetectionByAffiliation.bDetectEnemies=true;
	AiSenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies=false;
	AiSenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals=false;
	//SightRadius初始侦测范围，AI首次发现目标的“最大视距”
	AiSenseConfig_Sight->SightRadius=5000.f;
	//LoseSightRadius 目标脱离范围，AI在已经看见目标后，目标需要移动多远才会被判定为“跟丢”的半径
	AiSenseConfig_Sight->LoseSightRadius=0.f;
	//PeripheralVisionAngleDegrees  视野角度，定义了AI视野的水平夹角，
	AiSenseConfig_Sight->PeripheralVisionAngleDegrees=360.f;

	EnemyPerceptionComponent=CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	//将AiSenseConfig_Sight绑定给EnemyPerceptionComponent
	EnemyPerceptionComponent->ConfigureSense(*AiSenseConfig_Sight);
	//SetDominantSense 用于为AI感知组件指定一个“主导感官”，解决空间定位冲突。
	//除了这里设置的视觉感官，还存在听觉等其他感官，如果存在多个感官，不同感官对目标的感知就可能不同，从而产生冲突
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	//OnTargetPerceptionUpdated 是一个多播委托（动态多播事件），它在任何已注册的刺激源（Stimulus）状态发生更新时被触发。
	//其调用规则包括：敌人首次进入视野，敌人持续在视野内时，其感知信息（如位置）被更新，敌人离开视野都会被调用
	//其委托回调函数，存在两个参数，一个Actor代表感知的对象，一个Stimulus代表感知信息类
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this,&ThisClass::OnEnemyPerceptionUpdated);

	//最好不要在构造函数中设置，可以在BeginPlay等
	SetGenericTeamId(FGenericTeamId(1));
}

//游戏启动时调用一次，一个 Actor（刺激源）进入、离开或处于某个 AI 的感知范围边缘时调用一次，听觉感知可能例外不会调用（能够直接比较TeamId）
ETeamAttitude::Type AWarriorAIController::GetTeamAttitudeTowards(const AActor& Other) const
{




	//将AActor参数转化为团队接口IGenericTeamAgentInterface
	const APawn* PawnToCheck=Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface* OtherTeamAgent=Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());


	//如果玩家（Other）与敌人的TeamId不同，那么判定玩家为我们敌人的敌人
	if (OtherTeamAgent&&OtherTeamAgent->GetGenericTeamId()!=GetGenericTeamId())
	{

		return  ETeamAttitude::Hostile;
	}



	return  ETeamAttitude::Friendly;
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{


}
