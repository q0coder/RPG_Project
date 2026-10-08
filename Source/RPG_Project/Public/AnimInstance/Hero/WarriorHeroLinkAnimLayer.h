// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkAnimLayer.generated.h"


class UWarriorHeroAnimInstance;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorHeroLinkAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
};
