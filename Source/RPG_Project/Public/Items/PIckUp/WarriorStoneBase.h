// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PIckUp/WarriorPickBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API AWarriorStoneBase : public AWarriorPickBase
{
	GENERATED_BODY()
public:
	void Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent,int32 ApplyLevel);
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneConsumeGameplayEffect;

	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="On Stone Consumed"))
	void BP_OnStoneConsumed();
};
