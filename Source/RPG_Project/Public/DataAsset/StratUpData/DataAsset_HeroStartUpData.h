// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DataAsset/StratUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_HeroStartUpData.generated.h"


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
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
public :
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1) ;

private:
	UPROPERTY(EditDefaultsOnly,Category="StartUpData",meta=(TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;

};
