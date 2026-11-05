// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "GameFramework/SaveGame.h"
#include "WarriorSaveGame.generated.h"


/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	EWarriorGameDifficulty SaveCurrentGameDifficulty;

};
