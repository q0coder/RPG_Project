// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class RPG_PROJECT_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWarriorWeaponBase();

protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	UBoxComponent* WeaponCollisionBox;

public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }

};
