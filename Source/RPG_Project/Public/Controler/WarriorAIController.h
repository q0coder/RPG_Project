// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include"Perception/AIPerceptionTypes.h"


#include "WarriorAIController.generated.h"

class AIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()

public:
	//FObjectInitializer允许在对象构造期间干预其子对象（Subobject）的创建
	AWarriorAIController(const FObjectInitializer& ObjectInitializer);

	//用于判断两个Actor之间阵营关系（敌对、中立、友方） 的核心虚函数
	//~ Begin IGenericTeamAgentInterface Interface.
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~ End IGenericTeamAgentInterface Interface




protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAISenseConfig_Sight* AiSenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated( AActor* Actor, FAIStimulus Stimulus);
private:
	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance=true;

	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance",UIMin="1",UIMAX=4))
	int32 DetourCrowdAvoidanceQuality=4;

	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange=600.f;


};
