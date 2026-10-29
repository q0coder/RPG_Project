// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AbilityTask_WaitSpawnEnemies.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
	FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
	const FVector& SpawnOrigin, float RandomSpawnRadius, const FRotator& SpawnRoration)
{

	UAbilityTask_WaitSpawnEnemies* Node=NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag=EventTag;
	Node->CachedSoftEnemyClassToSpawn=SoftEnemyClassToSpawn;
	Node->CachedNumToSpawn=NumToSpawn;
	Node->CachedSpawnOrigin=SpawnOrigin;
	Node->CachedRandomSpawnRadius=RandomSpawnRadius;
	Node->CachedSpawnRoration=SpawnRoration;

	return Node;
}
