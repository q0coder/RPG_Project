// Fill out your copyright notice in the Description page of Project Settings.


#include "Controler/WarriorAIController.h"

#include <strmif.h>

#include "WarriorDebugHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
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


	//如果玩家ID（Other,0）与敌人的TeamId(1)，玩家Id小于敌人，那么玩家就被判定为敌人的敌人
	//修改不使用！=的原因是因为EQS 本身不是 Actor，但是 EQS的测试Pawn类会被感知（引擎对于所有的Pawn类都会默认注册为刺激源），如果没有对其分配Id，默认的NoTeamID=255
	//NoTeamID并不是所有的Actor都默认的TeamID，只有当某个对象参与 TeamID 判断、且它没有有效 TeamID 时，语义上视为 NoTeam。
	if (OtherTeamAgent&&OtherTeamAgent->GetGenericTeamId()<GetGenericTeamId())
	{

		return  ETeamAttitude::Hostile;
	}



	return  ETeamAttitude::Friendly;
}

void AWarriorAIController::BeginPlay()
{
	Super::BeginPlay();
	if (UCrowdFollowingComponent* CrowdComponent=Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		//设置避障模拟开关，决定这个 AI 是否参与群体避障计算。
		CrowdComponent->SetCrowdSimulationState(bEnableDetourCrowdAvoidance?ECrowdSimulationState::Enabled:ECrowdSimulationState::Disabled);
		//选择避障质量（四种，越高越消耗CPU性能）
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1:
		CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);break;

		case 2:
		CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);break;

		case 3:
		CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);break;

		case 4:
		CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);break;

		default:break;
		}
		//设置所属避障组告诉系统这个 AI 属于哪个阵营/组别，这里实际上使用的是位掩码
		CrowdComponent->SetAvoidanceGroup(1);
		//设置一个避开的组告诉系统这个 AI 在移动时需要避开哪些组的成员。
		CrowdComponent->SetGroupsToAvoid(1);
		//设置碰撞查询范围设置 AI 在避障时向周围“看”多远
		CrowdComponent->SetCrowdCollisionQueryRange(CollisionQueryRange);


	}
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

		if (UBlackboardComponent* BlackboardComponent=GetBlackboardComponent())
		{
			//只有在值未设置时会继续值的设置，不会在每次调用时都重新设置
			if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
			{
				//Stimulus.WasSuccessfullySensed()用于判断一次感知刺激当前是否被成功感知，返回一个 bool 值。
				if (Stimulus.WasSuccessfullySensed()&&Actor)
				{
					//设置BlackBoard中对应类型的键的值，参数接收一个键名和一个值（键名必须与BlackBoard中定义的相同，值必须为定义的键的基类类型）
					//BlackBoard的基类选项作用就是限制该 Object 键只能存储特定类型的对象
					BlackboardComponent->SetValueAsObject(FName("TargetActor"),Actor);
				}
			}

		}


}
