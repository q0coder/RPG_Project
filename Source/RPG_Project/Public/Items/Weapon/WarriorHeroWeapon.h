// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapon/WarriorWeaponBase.h"
#include "WarriorTypes//WArriorStructTypes.h"
#include "WarriorHeroWeapon.generated.h"

/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WeaponData")
	FWarriorHeroWeaponData HeroWeaponData;

};
