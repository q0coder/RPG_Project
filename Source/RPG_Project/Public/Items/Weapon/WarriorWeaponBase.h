// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInterfaceDelegate,AActor*);

UCLASS()
class RPG_PROJECT_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWarriorWeaponBase();
	FOnTargetInterfaceDelegate OnWeaponHitTarget;
	FOnTargetInterfaceDelegate OnWeaponPulledFromTarget;

protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UBoxComponent* WeaponCollisionBox;

	//设计为虚函数，子类可能重写
	//AddDynamic添加的回调函数必须是UFUNCTION的
	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }

};
