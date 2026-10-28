// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UBoxComponent;
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


//#if WITH_EDITOR ... #endif 之间的代码只会被编译进编辑器版本，打包进游戏时会被预处理器整段剔除。
#if WITH_EDITOR
	//~ Begin UObject Interface.
	//PostEditChangeProperty 是仅编辑器可用的虚函数，在详情面板（Details panel）中修改属性之后（或由编辑器驱动的其他属性变更之后）由引擎调用。
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface.

#endif



	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	UEnemyCombatComponent* EnemyCombatComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	UBoxComponent* LeftHandBoxCollision;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	UBoxComponent* RightHandBoxCollision;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	FName LeftHandBoxCollisionAttachBoneName;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	FName RightHandBoxCollisionAttachBoneName;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;

	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);



private:
	void InitEnemyStartUpData();
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
	FORCEINLINE UBoxComponent* GetLeftHandBoxCollisionComponent() const { return LeftHandBoxCollision; }
	FORCEINLINE UBoxComponent* GetRightHandBoxCollisionComponent() const { return RightHandBoxCollision; }

};
