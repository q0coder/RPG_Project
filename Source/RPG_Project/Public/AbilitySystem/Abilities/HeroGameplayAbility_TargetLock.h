// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UWarriorWidgetBase;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void TryLockOnTarget();

	void GetAvailableActorToLock();

	void CancelTargetLockAbility();

	void CleanUP();

	void DrawTargetLockedWidget();

	void SetTargetLockWidgetPosition();

	AActor* GetNearestTargetFormAvailableActors(const TArray<AActor*>& InAvailableActors);

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	float BoxTraceDistance=5000.f;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	FVector TraceBoxSize=FVector(5000.f,5000.f,300.f);

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	 TArray<TEnumAsByte<EObjectTypeQuery> >  BoxTraceChannel;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	bool bShowPersistantDebugType=false;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	UPROPERTY()
	AActor* CurrentLockedActor;

	UPROPERTY()
	UWarriorWidgetBase* DrawTargetLockWidget;

	UPROPERTY()
	FVector2D TargetLockWidgetSize;

};
