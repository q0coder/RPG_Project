// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WarriorGameModeBase.h"
#include "WarriorSurvivalGameMode.generated.h"

class AWarriorEnemyCharacter;

UENUM(BlueprintType)
enum class EWarriorSurvivalGameModeState:uint8
{
	WaitSpawnNewWave,
	SpawningNewWave,
	InProgress,
	WaveComplete,
	AllWavesDone,
	PlayerDied
};

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	UPROPERTY(EditDefaultsOnly)
	int32 MinPerSpawnCount=1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxPerSpawnCount=3;

};

//FTableRowBase 是虚幻引擎中 DataTable（数据表格） 系统的核心基类。几乎所有需要利用 CSV 或 JSON 文件批量配置游戏数据的结构体，都必须继承自它
//继承的结构体中定义的属性想要暴露给编辑器必须加上UPROPERTY，定义的属性都是列名
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow:public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	UPROPERTY(EditDefaultsOnly)
	int32 TotalEnemyToSpawnThisWave=1;


};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameModeStateChangedDelegate, EWarriorSurvivalGameModeState,CurrentState);
/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorSurvivalGameMode : public AWarriorGameModeBase
{
	GENERATED_BODY()

protected:
	//C++ BeginPlay 和蓝图中的 Event BeginPlay 是两个不同的函数,Event BeginPlay 在C++层面实际对应的是 ReceiveBeginPlay() 这个函数
	//AActor::BeginPlay() 是C++中的一个 virtual 虚函数，它内部会显式地调用 ReceiveBeginPlay()
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState);

	bool HasFinishedAllWaves() const;

	void PreLoadNextWaveEnemies();

	int32 TrySpawnWaveEnemies();

	bool ShouldKeepSpawnEnemies() const;

	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);


	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;

	UPROPERTY()
	EWarriorSurvivalGameModeState CurrentSurvivalGameModeState;

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnSurvivalGameModeStateChangedDelegate OnSurvivalGameModeStateChanged;

	//UDataTable 的核心属性 RowStruct 明确要求，它指向的结构体必须继承自 FTableRowBase，否则引擎无法将其识别为有效的数据表行结构。
	//为数据表定义的每一个 USTRUCT（如 FItemRow），都必须以 public FTableRowBase 作为基类。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	int32 TotalWaveToSpawn;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	float SPawnNewWaveWaitTime=5.f;

	UPROPERTY()
	float TimePassedSinceStart;

	UPROPERTY()
	int32 CurrentSpawnedEnemyCount=0;

	UPROPERTY()
	int32 TotalSpawnedEnemyCount=0;

	UPROPERTY()
	TArray<AActor*> TargetPointsArray;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	int32 CurrentWaveCount=1;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	float SpawnEnemiesDelayTime=2.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinitions",meta=(AllowPrivateAccess="true"))
	float WaveCompleteWaitTime=5.f;

	UPROPERTY()
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>,UClass*> PreLoadEnemiesClassMap;

	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToSpawn);
};
