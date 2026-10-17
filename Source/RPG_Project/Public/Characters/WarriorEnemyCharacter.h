// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UWidgetComponent;
class UEnemyUIComponent;
class UEnemyCombatComponent;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
public:
	AWarriorEnemyCharacter();
	//~ Begin PawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End PawnCombatInterface Interface

	//~ Begin PawnUIInterface Interface.
	virtual  UPawnUIComponent* GetPawnUIComponent() const override;
	//~ End PawnUIInterface Interface

	//~ Begin PawnUIInterface Interface.
	virtual  UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End PawnUIInterface Interface

protected:
	virtual void BeginPlay() override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface



	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	UEnemyCombatComponent* EnemyCombatComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;
private:
	void InitEnemyStartUpData();
public:
	FORCEINLINE UEnemyCombatComponent* GetHeroCombatComponent() const { return EnemyCombatComponent; }
};
