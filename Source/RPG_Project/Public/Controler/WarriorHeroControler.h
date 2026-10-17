// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "GameFramework/PlayerController.h"
#include "WarriorHeroControler.generated.h"


/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorHeroController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AWarriorHeroController();

	//蓝图接口函数，取一个 Actor 所属的团队 ID
	// 引擎负责在感知系统初始化（早于 BeginPlay）调用一次，其他一般属于手动调用
	//~ Begin IGenericTeamAgentInterface Interface.
	virtual  FGenericTeamId GetGenericTeamId()  const override;
	//~ End IGenericTeamAgentInterface Interface


private:
	FGenericTeamId HeroTeamId;

};
