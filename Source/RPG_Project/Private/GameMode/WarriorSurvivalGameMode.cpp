// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/WarriorSurvivalGameMode.h"

#include "NavigationSystem.h"
#include "WarriorBlueprintFunctionLibrary.h"
#include "WarriorDebugHelper.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();
	ensureMsgf(EnemyWaveSpawnerDataTable, TEXT("EnemyWaveSpawnerDataTable is not assigned."));

	SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);

	TotalWaveToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();
}

void AWarriorSurvivalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;
		if (TimePassedSinceStart >= SPawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{

			CurrentSpawnedEnemyCount+=TrySpawnWaveEnemies();
			TimePassedSinceStart = 0.f;

			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}

	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaveComplete)
	{
		TimePassedSinceStart += DeltaSeconds;

		if (TimePassedSinceStart >= WaveCompleteWaitTime)
		{
			TimePassedSinceStart = 0.f;
			++CurrentWaveCount;

			if (HasFinishedAllWaves())
			{
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::AllWavesDone);
			}
			else
			{
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWarriorSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	EWarriorGameDifficulty SavedGameDifficulty;
	if (UWarriorBlueprintFunctionLibrary::LoadCurrentGameDifficulty(SavedGameDifficulty))
	{
		CurrentGameDifficulty=SavedGameDifficulty;
	}
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWaveToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves())
	{
		return;;
	}
	PreLoadEnemiesClassMap.Empty();
	for (const FWarriorEnemyWaveSpawnerInfo& EnemyWaveSpawnerInfo : GetCurrentWaveSpawnerTableRow()->
	     EnemyWaveSpawnerDefinitions)
	{
		if (EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this,EnemyWaveSpawnerInfo]()
			{
				if (UClass* LoadedEnemyClass = EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.Get())
				{
					PreLoadEnemiesClassMap.Emplace(EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
				}
			})
		);
	}
}

int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	ensureMsgf(!TargetPointsArray.IsEmpty(),
	           TEXT("No TargetPoints found in the level. Please add at least one TargetPoint actor to the level."));


	uint32 EnemiesSpawnedThisWave = 0;

	for (const FWarriorEnemyWaveSpawnerInfo& EnemyWaveSpawnerInfo : GetCurrentWaveSpawnerTableRow()->
	     EnemyWaveSpawnerDefinitions)
	{
		if (EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		const int32 NumToSpawn = FMath::RandRange(EnemyWaveSpawnerInfo.MinPerSpawnCount,
		                                          EnemyWaveSpawnerInfo.MaxPerSpawnCount);

		UClass* LoadedEnemyClass = PreLoadEnemiesClassMap.FindChecked(EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; ++i)
		{

			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnLocation = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotator = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation().
				ToOrientationRotator();

			FVector RandomLocation;

			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnLocation, RandomLocation, 400.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);

			AWarriorEnemyCharacter* SpawnEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(
				LoadedEnemyClass, RandomLocation, SpawnRotator, SpawnParameters);

			if (SpawnEnemy)
			{
				SpawnEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
				++EnemiesSpawnedThisWave;
				++TotalSpawnedEnemyCount;
			}

			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisWave;
			}
		}
	}

	return EnemiesSpawnedThisWave;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemyCount < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemyCount--;
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemyCount+=TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemyCount==0)
	{
		CurrentSpawnedEnemyCount=0;
		TotalSpawnedEnemyCount=0;
		SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaveComplete);
	}
	{

	}
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(
		RowName, FString());

	ensureMsgf(FoundRow, TEXT("Could not find row %s in EnemyWaveSpawnerDataTable"), *RowName.ToString());

	return FoundRow;
}

void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToSpawn)
{
	for ( AWarriorEnemyCharacter* SpawnedEnemy:InEnemiesToSpawn)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemyCount++;
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
		}
	}
}
