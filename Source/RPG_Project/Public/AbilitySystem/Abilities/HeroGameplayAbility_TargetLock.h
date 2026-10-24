// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UInputMappingContext;
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

	void InitTargetLockMovement();

	void InitTargetLockMappingContext();

	void CleanUP();

	void ResetTargetLockMovement();

	void ResetTargetLockMappingContext();

	void DrawTargetLockedWidget();

	void SetTargetLockWidgetPosition();

	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,TArray<AActor*>& OutActorsOnRight);

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

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	float TargetLockRotateInterpSpeed=5.f;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	float TargetLockMaxWalkSpeed=150.f;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	UInputMappingContext* TargetLockMappingContext;

	UPROPERTY(EditDefaultsOnly,Category="Ability|TargetLock")
	float TargetLockCameraOffestDistance=20.f;

	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);

	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	UPROPERTY()
	AActor* CurrentLockedActor;

	UPROPERTY()
	UWarriorWidgetBase* DrawTargetLockWidget;

	UPROPERTY()
	FVector2D TargetLockWidgetSize;

	UPROPERTY()
	 float CachedDefaultMaxWalkSpeed=0.f;

};
