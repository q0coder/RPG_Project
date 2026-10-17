// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeaponChangedDelegate,TSoftObjectPtr<UTexture2D>,SoftWeaponIconTexture);

/**
 *
 */
UCLASS()
class RPG_PROJECT_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnPersentChangedDelegate OnCurrentRageChanged;
	//在蓝图在可调用，在蓝图的装备与卸下武器中进行广播
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnEquipWeaponChangedDelegate OnEquipWeaponChanged;


};
