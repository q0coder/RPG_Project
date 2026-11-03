// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AWarriorGameModeBase();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GameplayDifficulty")
	EWarriorGameDifficulty CurrentGameDifficulty;

public:
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const {return CurrentGameDifficulty;}

};
