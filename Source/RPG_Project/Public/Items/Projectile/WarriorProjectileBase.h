// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "WarriorProjectileBase.generated.h"

struct FGameplayEventData;
class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraComponent;


UENUM(BlueprintType)
enum class EProjectileDamagePolicy:uint8
{
	OnHit,
	OnBeginOverlap
};

UCLASS()
class RPG_PROJECT_API AWarriorProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AWarriorProjectileBase();

protected:
	virtual void BeginPlay() override;

UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UBoxComponent* ProjectileCollisionBox;

UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;

UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent;


UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	EProjectileDamagePolicy  ProjectileDamagePolicy=EProjectileDamagePolicy::OnHit;

UPROPERTY(BlueprintReadOnly,Category="Projectile",meta=(ExposeOnSpawn="true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;


	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="On Spawn Projectile Hit Fx "))
	void BP_OnSpawnProjectileHitFX(const FVector& HitPosition);

	UFUNCTION()
	virtual  void OnProjectileHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad);
};
