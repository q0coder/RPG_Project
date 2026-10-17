// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"


#include "WArriorStructTypes.generated.h"

class UInputMappingContext;
class UWarriorGameplayAbility;
class UWarriorHeroLinkAnimLayer;


USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	 UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Category="InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

	bool IsValid() const;


};
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
  {
  	GENERATED_BODY()

  	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
  	TSubclassOf<UWarriorHeroLinkAnimLayer> WeaponAnimLayerToLink;

  	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
  	UInputMappingContext* WeaponInputMappingContext;

  	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
  	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;


  	//FScalableFloat 是 GAS中用于实现数据驱动、等级化数值成长的核心结构体。它把“基础值”和“曲线表绑定在一起，
  	//同一个技能或效果在不同等级下自动产生不同的数值
  	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
  	FScalableFloat WeaponBaseDamage;

  	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftIconTextureWeapon;

  };
