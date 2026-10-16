// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

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

};
